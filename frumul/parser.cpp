#include <cassert>
#include "parser.h"

namespace frumul {
 	// constructors
	Parser::Parser (const bst::str& nsource, const bst::str& nfilepath) :
		source{nsource}, filepath{nfilepath},
		lex{nsource,nfilepath},
		AST{Node::DOCUMENT,Position{0,nsource.uLength()-1,nfilepath,source},StrNodeMap()}
	{
		current_token = new Token{lex.getNextToken(Token::MAX_TYPES_HEADER)};
	}

	Parser::~Parser() {
		if (current_token)
			delete current_token;
	}
	// other public functions

	Node& Parser::parse () {
		/* Parses the source file 
		 * and return the document Node
		 */
		if (!alreadyparsed) {
			document();
			alreadyparsed = true;
		}
		return AST;
	}
	

	// private functions
	
	int Parser::getTokenStart () const {
		/* return the position of the start
		 * of the current_token
		 * Crashes if no current token exists
		 */
		assert(current_token&&"Current token points to nothing");
		return current_token->getPosition().getStart();
	}

	template <typename ...T>
		bool Parser::eat(Token::Type t, T ...expected) {
			return _eat(t, {expected...});
		}

	bool Parser::_eat(Token::Type t, std::initializer_list<Token::Type> expected) {
		/* compare current token with
		 * t, and tries to get a new token
		 * delete current_token and set a new one
		 * return false if type does not match.
		 */
#if DEBUG
		if (current_token->getType() != Token::EOFILE)
			std::cout << *current_token << std::endl;
#endif
		if (current_token->getType() == t) {
			delete current_token;
			current_token = new Token{lex.getNextToken(expected)};
			return true;
		}
#if DEBUG
		std::cout << *current_token << std::endl;
		std::cout << __LINE__ << std::endl;
#endif
		throw BaseException(BaseException::UnexpectedToken,"Token expected: "+Token::typeToString(t),Position(current_token->getPosition()));
		return false;
	}

	// parser core 
	
	Node Parser::document() {
		/* return a document Node
		 * Position of this node is managed
		 * directly by the constructor
		 * of the parser
		 * No value linked to the node, but two fields:
		 * header and text
		 */
		AST.addChild("header",header());
		AST.addChild("text",text());
		return AST;
	}

	Node Parser::header() {
		/* manages the header of the document
		 * Return the header node
		 * The fields of the node are the statement list
		 */
		int start{current_token->getPosition().getStart()};

		// eat ___header___
		eat(Token::HEADER,Token::ID,Token::MAX_TYPES_HEADER);
		// get the statement list
		std::map<bst::str,Node> stlist {statement_list()};

		int end {current_token->getPosition().getStart() - 1};
		return Node{Node::HEADER,Position(start,end,filepath,source),
			stlist};

	}

	std::map<bst::str,Node> Parser::statement_list(bool isNamespace) { // lui faire passer un bool si namespace ou non
		/* Manages the list of declarations
		 * Return a map of declaration, with key
		 * as a digit
		 * isNamespace defines whether it is inside a namespace or not
		 * default: false
		 */
		std::map<bst::str,Node> statements;
		if (current_token->getType() != Token::ID) {
			int nodepos {current_token->getPosition().getStart()};
			statements.insert({bst::str(0),Node(Node::EMPTY,Position(nodepos,nodepos,filepath,source))});
		}
		// get declaration each after the other
		for (int i{0};current_token->getType() == Token::ID; ++i) {
			statements.insert({i,declaration()});

			if (!isNamespace)
				eat(Token::RAQUOTE,Token::ID,Token::MAX_TYPES_HEADER); 
			else
				eat(Token::RAQUOTE,Token::ID,Token::RPAREN,Token::MAX_TYPES_HEADER);
		}


		return statements;
	}

	Node Parser::declaration () {
		/* Manages the declarations.
		 * Return a Node with following fields:
		 * options,value.
		 * the name is saved in the value
		 */
		int start {current_token->getPosition().getStart()}; // start position

		bst::str name{current_token->getValue()}; // get name
		eat(Token::ID,Token::COLON,Token::MAX_TYPES_HEADER); // eat id
		eat(Token::COLON,				// eat declare op
				Token::LAQUOTE,			// can expect value
				Token::KEYWORD,			// or a keyword like mark, etc.
				Token::MAX_TYPES_HEADER); 

		std::map<bst::str,Node> fields;
		fields.insert({"options",options()});

		int end; // should be deleted TODO
		if (current_token->getType() == Token::LAQUOTE) { // we can assume it is a basic value
			int start {getTokenStart()};
			eat(Token::LAQUOTE,Token::VAL_TEXT,Token::LBRACE,Token::MAX_TYPES_VALUES); // consume «
			std::cout << *current_token << std::endl;
			fields.insert({"value",basic_value(start)});
			//RAQUOTE is eat in statement_list
			end = fields.at("value").getPosition().getEnd(); // TODO should be deleted
		}

		//int end {fields.at("value").getPosition().getEnd()}; // end position


		return Node{Node::DECLARATION,Position(start,end,filepath,source),fields,name};
	}

	Node Parser::basic_value (int start) {
		/* Manages basic value
		 * Return a node with the basic value
		 * Node has no value, but the fields are
		 * filled with nodes ordered by digits
		 * start is the position of the LAQUOTE
		 */
		std::vector<Node> fields;
		while (current_token->getType() != Token::RAQUOTE && current_token->getType() != Token::EOFILE) {
			// simple text
			if (current_token->getType() == Token::VAL_TEXT) {
				fields.push_back(Node(Node::VAL_TEXT,current_token->getPosition(),current_token->getValue()));
				eat(Token::VAL_TEXT,Token::LBRACE,Token::MAX_TYPES_VALUES);
			}
			// programmatic part
			else if (current_token->getType() == Token::LBRACE) {
				eat(Token::LBRACE,Token::MAX_TYPES_VALUES); // eat {
				if (in<bst::str,std::initializer_list<bst::str>>(current_token->getValue(),{"pool","else","fi"}))
					break; // end of a loop or a condition
				fields.push_back(programmatic_part());
				eat(Token::RBRACE,Token::VAL_TEXT,Token::MAX_TYPES_VALUES); // eat }
			}
		}
		int end {current_token->getPosition().getEnd()}; // should match with the position of RAQUOTE
		return Node(Node::BASIC_VALUE,Position(start,end,filepath,source),fields);

	}

	Node Parser::programmatic_part () {
		/* This function manages the
		 * programmatic parts inside
		 * the values
		 * Node returned may be of various types.
		 */
		if (current_token->getType() == Token::RBRACE)
			return Node{Node::EMPTY,current_token->getPosition()};

		if (current_token->getType() != Token::VARIABLE) // we know it is not a statement
			return expr(); 

		if (current_token->getValue() == "if")
			assert(false&&"If statement is not yet set");

		if (current_token->getValue() == "loop")
			return loop();

		// we should now consider wether it is:
		// - a declaration
		// - an assignment
		// - an expression starting by a variable
		
		Token nextToken {lex.peekToken(0,Token::MAX_TYPES_VALUES)};

		// variable declaration without assignment
		if (nextToken.getType() == Token::COMMA)
			return variable_declaration();

		// an expression starting by a variable
		if (nextToken.getType() != Token::ASSIGN)
			return expr();

		// we can assume there is an assignment here
		Node assign_node {variable_assignment()};

		// check if it is a variable declaration with assignment
		if (current_token->getType() == Token::COMMA) {
			// we need to get the type
			eat(Token::COMMA,Token::MAX_TYPES_VALUES); // eat ,
			assign_node.addChild("value",Node(Node::VARIABLE_TYPE,current_token->getPosition(),current_token->getValue()));
			eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat type

			int start{assign_node.getPosition().getStart()};
			int end{current_token->getPosition().getEnd()};

			return Node{ Node::VARIABLE_DECLARATION,Position(start,end,filepath,source),assign_node.getNamedChildren()};
		}

		return assign_node;

	}

	Node Parser::variable_declaration () {
		/* Manages every variable declaration
		 * Return a node with
		 * three fields:
		 * name, type,
		 * and a value (optional)
		 */
		int start{getTokenStart()};
		StrNodeMap fields;
		fields.insert({"name" , Node(Node::VARIABLE_NAME,current_token->getPosition(),current_token->getValue())});
		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat name
		if (current_token->getType() == Token::ASSIGN) {
			eat(Token::ASSIGN,Token::MAX_TYPES_VALUES); // eat :
			fields.insert({"value" , expr()});
		}
		eat(Token::COMMA,Token::MAX_TYPES_VALUES); // eat ,
		fields.insert({"type" , Node(Node::VARIABLE_TYPE,current_token->getPosition(),current_token->getValue())});
		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat type
		int end{current_token->getPosition().getEnd()};
		return Node(Node::VARIABLE_DECLARATION,Position(start,end,filepath,source),fields);
	}

	Node Parser::variable_assignment () {
		/* Manages the variable assignment
		 * TODO
		 */
		return Node(Node::MAX_TYPES,Position(1,1,filepath,source),"");
	}

	Node Parser::comparison () {
		/* Manages the comparison
		 * It can return a comparison if it found one,
		 * or an expression.
		 */
		Node expression { expr() };
		if (!intokl(current_token->getType(),{Token::EQUAL,Token::GREATER,Token::LESS}))
			return expression;

		int start { expression.getPosition().getStart() };
		NodeVector fields;
		fields.push_back(expression);

		while (true) {
			bst::str val;
			int start{getTokenStart()};
			int end {start};
			switch (current_token->getType()) {
				case Token::EQUAL:
					val = "=";
					end = start;
					eat(current_token->getType(),Token::MAX_TYPES_VALUES);
					break;
				case Token::GREATER:
				case Token::LESS:
					val = current_token->getValue();
					eat(current_token->getType(),Token::MAX_TYPES_VALUES);
					if (current_token->getType() == Token::EQUAL) {
						val += "=";
						end = getTokenStart();
						eat(Token::EQUAL,Token::MAX_TYPES_VALUES);
					}
				default:
					break;
			};
			fields.push_back(Node{Node::COMPARE_OP,Position(start,end,filepath,source),val});
			fields.push_back(expr());
		}

		int end { fields.back().getPosition().getEnd() };
		Node compare_node{Node::COMPARISON,Position(start,end,filepath,source),fields};
		return compare_node;
	}

	Node Parser::expr () {
		/* Manages all the expressions
		 * Return a node
		 * which can have a value
		 * and/or children
		 */

		Node* temp_node = new Node(term());

		while (intokl(current_token->getType(),{Token::PLUS,Token::MINUS,Token::OR})) {
			bst::str val;
			switch (current_token->getType()) {
				case Token::PLUS:
					val = "+";
					break;
				case Token::MINUS:
					val = "-";
					break;
				case Token::OR:
					val = "|";
					break;
				default: // f**** -Wswitch
					break;
			};

			auto binop = new Node(Node::BIN_OP,current_token->getPosition(),StrNodeMap(),val);
			eat(current_token->getType(),Token::MAX_TYPES_VALUES);
			Node term2 {term()};
			
			binop->addChild("right",*temp_node);
			binop->addChild("left",term2);

			delete temp_node;
			temp_node = new Node{*binop};
		}

		Node returned_node {*temp_node};
		delete temp_node;

		return returned_node;
	}

	Node Parser::term () {
		/* Manages all the term.
		 * return Node of various types
		 * Recursive function
		 */
		Node factor1 {factor()};

		bst::str val;
		if (intokl(current_token->getType(),{Token::MUL,Token::DIV,Token::MODULO,Token::AND}))
			val = current_token->getValue();
		else
			return factor1;

		Node binop {Node::BIN_OP,current_token->getPosition(),StrNodeMap(),val};
		eat(current_token->getType(),Token::MAX_TYPES_VALUES);

		binop.addChild("right",factor1);
		binop.addChild("left",term());

		return binop;
	}

	Node Parser::factor () {
		/* Manages all factor.
		 * return Node of various types
		 * Recursive function.
		 */
		// manages unary op: -,+,!
		if (intokl(current_token->getType(),{Token::MINUS,Token::MUL,Token::NOT})) {
			Node unop{Node::UNARY_OP,current_token->getPosition(),{{"expr",factor()}},current_token->getValue()};

			eat(current_token->getType(),Token::MAX_TYPES_VALUES);
			return unop;
		}

		// text, number and bool litteral
		if (intokl(current_token->getType(), {Token::NUMBER,Token::LAQUOTE}) || in<bst::str,std::initializer_list<bst::str>>(current_token->getValue(),{"true","false"}))
			return litteral();

		switch (current_token->getType()) {
			case Token::PARENT:
				// symbol call or alias litteral
				assert(false&&"parent_expr not yet set");
				//return parent_expr()
			case Token::LBRACKET:
				// list litteral
				assert(false&&"list not yet set");
				//return list();
			case Token::LPAREN:
				{
				eat(Token::LPAREN,Token::MAX_TYPES_VALUES);
				Node expression {expr()};
				eat(Token::RPAREN,Token::MAX_TYPES_LANG_VALUES);
				return expression;
				}
			default: // -Wswitch again
				break;
		};
		// default: reference
		return reference();
	}

	Node Parser::reference () {
		/* Manages the references:
		 * a variable or a list index
		 * if it is a variable, return a node
		 * with a value only;
		 * else with a child named "index"
		 */
		int start {getTokenStart()};
		int end {current_token->getPosition().getEnd()};

		bst::str variable_name {current_token->getValue()};

		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES);

		if (current_token->getType() == Token::LBRACKET) {
			eat(Token::LBRACKET,Token::MAX_TYPES_VALUES);
			Node index{expr()};
			eat(Token::RBRACKET,Token::MAX_TYPES_VALUES);
			end = index.getPosition().getEnd();
			return Node {Node::VARIABLE_NAME,Position(start,end,filepath,source),{{"index",index}},variable_name};
		}

		return Node {Node::VARIABLE_NAME,Position(start,end,filepath,source),variable_name};
	}

	Node Parser::loop () {
		/* Manages the loop
		 * Return a loop Node
		 * which has two children:
		 * the expression/number of times
		 * the loop should be called;
		 * and the text that will be printed
		 * or parts that will be executed
		 */
		int start {getTokenStart()};
		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat 'loop'
		// get comparison/number of times
		Node condition { comparison() };
		eat(Token::RBRACE,Token::VAL_TEXT,Token::LBRACE,Token::MAX_TYPES_VALUES); // eat }
		Node inside_loop {basic_value(getTokenStart()) };
		if (current_token->getValue() != "pool")
			throw BaseException(BaseException::UnexpectedToken,"The keyword 'pool' was expected to close the loop",Position(start,current_token->getPosition().getEnd(),filepath,source));

		std::cout << __LINE__ << std::endl;
		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat pool
		// RBRACE is eat by programmatic_part
		int end {getTokenStart()};

		return Node {Node::LOOP,Position(start,end,filepath,source),{{"condition",condition},{"text",inside_loop}}};
	}



	Node Parser::options () {
		/* Manages all the options:
		 * lang, mark and arg 
		 * Return a Node with no value
		 * but fields named with options
		 * names.
		 */
		int start {getTokenStart()}; // start position
		std::array<bst::str,3> optionsnames {"lang","mark","arg"};
		NodeVector fields;
		for (int i{0};in<bst::str,std::array<bst::str,3>>(current_token->getValue(),optionsnames);++i) {
				if (current_token->getValue() == "lang") {
					for (const auto& child : lang_option())
						fields.push_back(child.second);
					optionsnames[0] = "";
				}

				else if (current_token->getValue() == "mark") {
					fields.push_back(mark_option());
					optionsnames[1] = "";
				}

				else if (current_token->getValue() == "arg") {
					for (const auto& child : param_option())
						fields.push_back(child);
					optionsnames[2] = "";
				}

				eat(Token::RAQUOTE,Token::LAQUOTE,Token::ID,Token::MAX_TYPES_HEADER); // consume the end of each option: », and expects either « or and id
		}
		int end { fields.back().getPosition().getEnd() };// end position

		return Node(Node::OPTIONS,Position(start,end,filepath,source),fields);
	}

	Node Parser::mark_option () {
		/* Manages the mark option.
		 * Return a node with a value
		 * but no children
		 */
		int start {getTokenStart()};
		eat(Token::ID,Token::LAQUOTE,Token::MAX_TYPES_HEADER); // consume "mark"
		eat(Token::LAQUOTE,Token::NUMBER,Token::MAX_TYPES_VALUES); // consume «
		bst::str value { current_token->getValue()};
		eat(Token::NUMBER,Token::RAQUOTE,Token::MAX_TYPES_HEADER); // consume number
		int end {current_token->getPosition().getEnd()};
		return Node(Node::MARK,Position(start,end,filepath,source),value);
	}

	std::map<bst::str,Node> Parser::lang_option () {
		/* Manages the lang option.
		 * Returns a map of lang Nodes.
		 * Each node inside has a value
		 * and no child
		 */
		eat(Token::ID,Token::LAQUOTE,Token::MAX_TYPES_HEADER); // consume lang
		eat(Token::LAQUOTE,Token::LANGNAME,Token::MAX_TYPES_LANG_VALUES); // consume «

		std::map<bst::str,Node> fields;
		for (int i{0}; current_token->getType() != Token::RAQUOTE;++i) {
			Node n {Node::LANG,current_token->getPosition(),current_token->getValue()};
			fields.insert({bst::str(i),n});
			eat(Token::LANGNAME,Token::VBAR,Token::RAQUOTE,Token::MAX_TYPES_LANG_VALUES);
			if (current_token->getType() == Token::VBAR)
				eat(Token::VBAR,Token::LANGNAME,Token::MAX_TYPES_LANG_VALUES);
		}
		return fields;
	}

	std::vector<Node> Parser::param_option () {
		/* Manages the parameters.
		 * Returns a map of param Nodes.
		 * No value.
		 */
		eat(Token::ID,Token::LAQUOTE,Token::MAX_TYPES_HEADER); // consume arg
		eat(Token::LAQUOTE,Token::VARIABLE,Token::MAX_TYPES_VALUES); // consume «
		NodeVector fields;
		while ( current_token->getType() != Token::RAQUOTE ) {
			fields.push_back(param_value ());
			if (current_token->getType() == Token::VBAR)
				eat(Token::VBAR,Token::VARIABLE,Token::MAX_TYPES_VALUES);
		}
		return fields;
	}

	Node Parser::param_value() {
		/* Manages one parameter.
		 * Return a Node
		 * with three fields and no value:
		 * variable_declaration,
		 * number of args, TODO
		 * choices TODO
		 */
		int start{getTokenStart()};
		StrNodeMap fields;
		fields.insert({"variable" , variable_declaration()});
		int end{current_token->getPosition().getStart() -1};

		return Node(Node::PARAM,Position(start,end,filepath,source),fields);
	}

	Node Parser::litteral() {
		/* Parses the litteral text, int
		 * and bool.
		 * Return a Node with 
		 * a value.
		 */
		switch (current_token->getType()) {
			case Token::NUMBER:
				{
					Node number {Node::LITINT,current_token->getPosition(),current_token->getValue()};
					eat(Token::NUMBER,Token::MAX_TYPES_VALUES);
					return number;
				}
			case Token::LAQUOTE:
				{
					int start {getTokenStart()};
					eat(Token::LAQUOTE,Token::LITTEXT,Token::MAX_TYPES_VALUES); // eat «
					bst::str val {current_token->getValue()};
					eat(Token::LITTEXT,Token::RAQUOTE,Token::MAX_TYPES_VALUES); // eat text itself
					int end {getTokenStart()};
					eat(Token::RAQUOTE,Token::MAX_TYPES_VALUES); // eat »
					return Node{ Node::LITTEXT,Position(start,end,filepath,source),val};
				}
			default:
				//-Wswitch !!!
				break;
		};

		// default: bool
		Node b00l {Node::LITBOOL,current_token->getPosition(),current_token->getValue()};
		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat false/true
		return b00l;
	}

	Node Parser::text() {
		/* parses the text
		 * Node returned has no value
		 * but a list of parts of the text
		 * ordered by digits
		 */
		int start {current_token->getPosition().getStart()};

		// ___text___
		eat(Token::TEXT,Token::MAX_TYPES_TEXT);

		std::map<bst::str,Node> children;

		int i{0};
		while (current_token->getType() != Token::EOFILE) {
			switch (current_token->getType()) {
				case Token::SIMPLE_TEXT:
					children.insert(
							{i,Node{Node::SIMPLE_TEXT,current_token->getPosition(),current_token->getValue()}}
						       );
					eat(Token::SIMPLE_TEXT,Token::MAX_TYPES_TEXT);
					break;
				case Token::TAG:
					break;
				default:
					throw BaseException(BaseException::UnexpectedToken,"Token expected:\n" + Token::typeToString(Token::SIMPLE_TEXT) + "\n" + Token::typeToString(Token::TAG) + "\n",Position(current_token->getPosition()));
			};
		}

		int end {current_token->getPosition().getEnd()-1};
		// EOF
		eat(Token::EOFILE);

		Node text{Node::TEXT,Position(start,end,filepath,source), children};

		if (i == 0)
			text.addChild(0,Node{Node::EMPTY,Position(start,end,filepath,source)});

		return text;
	}

} // namespace
