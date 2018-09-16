#include <cassert>
#include <experimental/filesystem>
#include <system_error>
#include "parser.h"

namespace fs = std::experimental::filesystem;

namespace frumul {
 	// constructors
	Parser::Parser (const bst::str& nsource, const bst::str& nfilepath, const Token::Type next_token,Transpiler* ntranspiler) :
		source{nsource}, filepath{nfilepath},
		lex{nsource,nfilepath},
		transpiler{ntranspiler},
		AST{Node::DOCUMENT,Position{0,nsource.uLength()-1,nfilepath,source},StrNodeMap()}
	{
		current_token = new Token{lex.getNextToken(next_token,Token::MAX_TYPES_HEADER)};
	}

	Parser::Parser (const bst::str& nsource, const bst::str& nfilepath,Transpiler& ntranspiler) :
		Parser{nsource,nfilepath,Token::MAX_TYPES_HEADER,&ntranspiler}
	{
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

	const Symbol& Parser::getHeaderSymbol() const {
		/* return a reference
		 * to the header symbol
		 * if it is set
		 */
		assert(header_symbol&&"Header symbol has not yet been set");
		return *header_symbol;
	}

	const Transpiler& Parser::getTranspiler() const {
		/* Return transpiler if it is set
		 */
		assert(transpiler&&"Transpiler not set");
		return *transpiler;
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
		if (false&&current_token->getType() != Token::EOFILE)
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
		Hinterpreter header_interpreter {AST.get("header")};
		//header_symbol = std::make_unique<Symbol>(header_interpreter.getSymbolTree());
		header_symbol = header_interpreter.getSymbolTree();
		lex.setOpeningTags(header_symbol->getChildrenNames());
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
		std::vector<Node> stlist {statement_list()};

		int end {current_token->getPosition().getStart() - 1};
		return Node{Node::HEADER,Position(start,end,filepath,source),
			stlist};

	}

	std::vector<Node> Parser::statement_list(bool isNamespace) { // lui faire passer un bool si namespace ou non
		/* Manages the list of declarations
		 * Return a map of declaration, with key
		 * as a digit
		 * isNamespace defines whether it is inside a namespace or not
		 * default: false
		 */
		std::vector<Node> statements;
		if (current_token->getType() != Token::ID) {
			int nodepos {current_token->getPosition().getStart()};
			statements.push_back(Node(Node::EMPTY,Position(nodepos,nodepos,filepath,source)));
		}
		// get declaration each after the other
		for (int i{0};current_token->getType() == Token::ID; ++i) {
			statements.push_back(declaration());

			Token::Type t;
			// we need to know if current_token is supposed to be ) or »
			// if the last statement is a namespace, we expect a ), but if it is included in a file, we expect »
			if (statements.back().get("value").type() == Node::NAMESPACE_VALUE && !statements.back().getNamedChildren().count("path"))
				t = Token::RPAREN;
			else
				t = Token::RAQUOTE;

			if (!isNamespace)
				eat(t,Token::ID,Token::MAX_TYPES_HEADER); 
			else
				// if we are inside a namespace, we can expect an ID or a RPAREN
				eat(t,Token::ID,Token::RPAREN,Token::MAX_TYPES_HEADER);
		}


		return statements;
	}

	Node Parser::declaration (const bool isNameRequired) {
		/* Manages the declarations.
		 * Return a Node with following fields:
		 * for all:
		 * 	name,options,value
		 * for namespace:
		 * 	statements
		 * for file:
		 * 	path	
		 * If isNameRequired is set, it tries to get a name
		 */
		int start {current_token->getPosition().getStart()}; // start position


		std::map<bst::str,Node> fields;

		if (isNameRequired) {
			bst::str name{current_token->getValue()}; // get name
			Node::Type t = (name.uLength() > 1 ? Node::LONG_NAME : Node::SHORT_NAME);
			fields.insert({"name",Node(t,current_token->getPosition(),name)});

			eat(Token::ID,Token::COLON,Token::MAX_TYPES_HEADER); // eat id
			eat(Token::COLON,				// eat declare op
					Token::LAQUOTE,			// can expect value
					Token::KEYWORD,			// or a keyword like mark, etc.
					Token::MAX_TYPES_HEADER); 
		}
		fields.insert({"options",options()});

		int end{0}; 
		if (current_token->getType() == Token::LAQUOTE) { // we can assume it is a basic value
			int start {getTokenStart()};
			eat(Token::LAQUOTE,Token::VAL_TEXT,Token::LBRACE,Token::MAX_TYPES_VALUES); // consume «
			fields.insert({"value",basic_value(start)});
			//RAQUOTE is eat in statement_list
			end = fields.at("value").getPosition().getEnd(); 
		}

		else if (current_token->getValue() == "namespace") { // namespace
			eat(Token::ID,Token::LAQUOTE,Token::MAX_TYPES_HEADER); // eat 'namespace'
			int start {getTokenStart()};
			eat(Token::LAQUOTE,Token::MAX_TYPES_NAMESPACE_VALUES); // eat «
			fields.insert({"value",namespace_value(start)});
			eat(Token::RAQUOTE,Token::LPAREN,Token::MAX_TYPES_HEADER); // eat »
			eat(Token::LPAREN,Token::ID,Token::MAX_TYPES_HEADER); // eat (
			NodeVector stlist {statement_list(true)};
			Node stmt_node{Node::NAMESPACE_STATEMENTS,
				Position(stlist.at(0).getPosition().getStart(),stlist.back().getPosition().getEnd(),filepath,source),stlist};
			fields.insert({"statements",stmt_node});
			//RPAREN is eat in statement_list
			end = current_token->getPosition().getEnd();
		}
		else if (current_token->getValue() == "file") { // file insertion
			eat(Token::ID,Token::LAQUOTE,Token::MAX_TYPES_HEADER); //eat 'file'
			Node path {path_value ()};
			fields.insert({"path",path});
			end = current_token->getPosition().getEnd();
			// RAQUOTE is eat in statement_list

			Node content {file_content(path)}; // return a declaration
			fields.insert({"value",content.get("value")});
			if (fields.at("value").type() == Node::NAMESPACE_VALUE)
				fields.insert({"statements",content.get("statements")});

			if (fields.at("options").type() == Node::EMPTY) {
				fields.erase("options");
				fields.insert({"options",content.get("options")});
			} else 
				for (const auto& elt : content.get("options").getNumberedChildren())
					fields.at("options").addChild(elt);
			//fields.at("value").removeChild("options");TODO REMOVE?


		} else if (current_token->getValue() == "alias") { // alias
			eat(Token::ID,Token::LAQUOTE,Token::MAX_TYPES_HEADER); // eat 'alias'
			eat(Token::LAQUOTE,Token::ID,Token::MAX_TYPES_HEADER); // eat '«' and get the symbol path
			Node value {Node::ALIAS_VALUE,current_token->getPosition(),current_token->getValue()};
			fields.insert({"value",value});
			eat(Token::ID,Token::RAQUOTE,Token::MAX_TYPES_HEADER); // eat path and get '»'
			end = current_token->getPosition().getEnd();
			// RAQUOTE is eat in statement_list
		} else
			assert(false&&"No option selected");
		// check 'end' value to avoid garbage
		assert(end&&"end has not been set");



		return Node{Node::DECLARATION,Position(start,end,filepath,source),fields};
	}

	Node Parser::basic_value (const int start) {
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
			return comparison(); 

		if (current_token->getValue() == "if")
			return condition();

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
			assign_node.addChild("type",Node(Node::VARIABLE_TYPE,current_token->getPosition(),current_token->getValue()));
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
			fields.insert({"value" , comparison()});
		}
		eat(Token::COMMA,Token::MAX_TYPES_VALUES); // eat ,
		fields.insert({"type" , Node(Node::VARIABLE_TYPE,current_token->getPosition(),current_token->getValue())});
		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat type
		int end{current_token->getPosition().getEnd()};
		return Node(Node::VARIABLE_DECLARATION,Position(start,end,filepath,source),fields);
	}

	Node Parser::variable_assignment () {
		/* Manages the variable assignment
		 * Return a node with two fields:
		 * a name and a value
		 */
		int start {getTokenStart()};
		StrNodeMap fields;
		//get name
		fields.insert({"name",Node{Node::VARIABLE_NAME,current_token->getPosition(),current_token->getValue()}});

		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat name
		// get assign
		eat(Token::ASSIGN,Token::MAX_TYPES_VALUES); // eat :
		// get value
		fields.insert({"value",comparison()});

		int end{fields.at("value").getPosition().getEnd()};
		return Node{Node::VARIABLE_ASSIGNMENT,Position(start,end,filepath,source),fields};
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

		bool isComparison{true};
		while (isComparison) {
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
					break;
				default:
					isComparison = false;
					break;
			};
			if (isComparison) {
				fields.push_back(Node{Node::COMPARE_OP,Position(start,end,filepath,source),val});
				fields.push_back(expr());
			}
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

			auto binop = new Node(Node::BIN_OP,current_token->getPosition(),StrNodeMap(),val); // binop is given to temp_node, wich is deleted at the end of the function
			eat(current_token->getType(),Token::MAX_TYPES_VALUES);
			Node term2 {term()};
			
			binop->addChild("left",*temp_node);
			binop->addChild("right",term2);

			delete temp_node;
			temp_node = binop;
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

		binop.addChild("left",factor1);
		binop.addChild("right",term());

		return binop;
	}

	Node Parser::factor () {
		/* Manages all factor.
		 * return Node of various types
		 * Recursive function.
		 */
		// manages unary op: -,+,!
		if (intokl(current_token->getType(),{Token::MINUS,Token::PLUS,Token::NOT})) {

			Position cur_pos{current_token->getPosition()};
			bst::str cur_val{current_token->getValue()};
			eat(current_token->getType(),Token::MAX_TYPES_VALUES);

			Node unop{Node::UNARY_OP,cur_pos,{{"expr",factor()}},cur_val};

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
				return list();
			case Token::LPAREN:
				{
				eat(Token::LPAREN,Token::MAX_TYPES_VALUES);
				Node expression {comparison()};
				eat(Token::RPAREN,Token::MAX_TYPES_VALUES);
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
		StrNodeMap fields;
		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat 'loop'
		// get comparison/number of times
		Node condition { comparison() };
		// get the iterable if it exists
		if (current_token->getType() == Token::ASSIGN) {
			eat(Token::ASSIGN,Token::MAX_TYPES_VALUES); // eat :
			Node iterable { comparison() };
			fields.insert({"variable",condition});
			fields.insert({"iterable",iterable});
		} else
			fields.insert({"condition",condition});
			
		eat(Token::RBRACE,Token::VAL_TEXT,Token::LBRACE,Token::MAX_TYPES_VALUES); // eat }
		Node inside_loop {basic_value(getTokenStart()) };
		fields.insert({"inside_loop",inside_loop});
		if (current_token->getValue() != "pool")
			throw BaseException(BaseException::UnexpectedToken,"The keyword 'pool' was expected to close the loop",Position(start,current_token->getPosition().getEnd(),filepath,source));

		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat pool
		// RBRACE is eat by programmatic_part
		int end {getTokenStart()};

		return Node {Node::LOOP,Position(start,end,filepath,source),fields};
	}

	Node Parser::condition () {
		/* Manages the condition.
		 * Return a condition Node
		 * with two or three children:
		 * - the condition
		 * - the text inside the block
		 * - the text inside the else block (optional)
		 */
		int start {getTokenStart()};
		StrNodeMap fields;
		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); //eat 'if'
		// get comparison
		fields.insert({"comparison",comparison()});
		eat(Token::RBRACE,Token::VAL_TEXT,Token::LBRACE,Token::MAX_TYPES_VALUES); //eat }
		// get text inside
		fields.insert({"text",basic_value(getTokenStart())});
		// get the else or pass
		if (current_token->getValue() == "else") {
			eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); //eat 'else'
			eat(Token::RBRACE,Token::VAL_TEXT,Token::LBRACE,Token::MAX_TYPES_VALUES); // eat }
			fields.insert({"else_text",basic_value(getTokenStart())});
		}
		// check if 'fi' is present
		if (current_token->getValue() != "fi")
			throw BaseException{BaseException::UnexpectedToken,"The keyword 'fi' was expected to close the conditional block",Position(start,current_token->getPosition().getEnd(),filepath,source)};

		eat(Token::VARIABLE,Token::MAX_TYPES_VALUES); // eat fi
		// RBRACE is eat by programmatic_part
		int end {getTokenStart()};

		return Node {Node::CONDITION,Position(start,end,filepath,source),fields};
	}

	Node Parser::path_value () {
		/* Manages the path value.
		 * Return a node with the path as value
		 * and no child.
		 */
		int start {getTokenStart()};
		eat(Token::LAQUOTE,Token::VAL_TEXT,Token::MAX_TYPES_VALUES); // eat «
		bst::str val {current_token->getValue()};
		eat(Token::VAL_TEXT,Token::MAX_TYPES_VALUES); // eat path
		int end {getTokenStart()};
		// RAQUOTE is eat in statement_list
		return Node{Node::PATH,Position(start,end,filepath,source),val};
	}

	Node Parser::file_content (const Node& path_node) {
		/* Manages the content of a header file
		 * Return a Declaration Node 
		 * TODO include standard lib
		 * TODO eat the last token manually: it can be a RPAREN (namespace)
		 * or a RAQUOTE (other values)
		 */
		// load file
		fs::path calling_file_path(reinterpret_cast<char*>(filepath.data));
		fs::path parent {calling_file_path.parent_path()};
		fs::path real_path {parent.append(reinterpret_cast<char*>(path_node.getValue().data))};
		try {
		Parser::files[path_node.getValue()] = readfile(real_path.native());
		} catch (std::system_error)
		{
			throw BaseException{exc::FileError,"Invalid file path.",path_node.getPosition()};
		}
		std::map<bst::str,bst::str>::iterator i{Parser::files.find(path_node.getValue())};
		const bst::str& path = i->first;
		// create parser
		Parser p{Parser::files[path],path,Token::ID};
		return p.declaration(false);
	}

	Node Parser::namespace_value (const int start) {
		/* Manages the namespace value.
		 * Return a Node NAMESPACE_VALUE
		 * with at least one child,
		 * long name, short name or linked name
		 * Children type: numbered_children
		 */
		NodeVector fields;
		while (current_token->getType() != Token::RAQUOTE) {
			switch (current_token->getType()) {
				case Token::SNAME:
					fields.push_back(short_name());
					break;
				case Token::LBRACE:
					fields.push_back(long_name());
					break;
				default:
					throw BaseException(BaseException::SyntaxError,"A short name or a long name is expected.",current_token->getPosition());
			};
		}
		int end{fields.back().getPosition().getEnd()};
		return Node{Node::NAMESPACE_VALUE,Position(start,end,filepath,source),fields};

	}

	Node Parser::short_name () {
		/* Manages the short name.
		 * Can return a short name
		 * (only a value)
		 * or a linked name
		 * (two children: short name
		 * and long name)
		 */	
		Node shortn{Node::SHORT_NAME,current_token->getPosition(),current_token->getValue()};
		eat(Token::SNAME,Token::MAX_TYPES_NAMESPACE_VALUES); // eat sname
		// get a potential long name associated
		if (current_token->getType() == Token::LBRACE) {
			Node longn {long_name()};
			Node linkedn {Node::LINKED_NAMES,Position(
					shortn.getPosition().getStart(),
					longn.getPosition().getEnd(),
					filepath,source),{{"short",shortn},{"long",longn}}
			};
			return linkedn;
		} 
		return shortn;
	}

	Node Parser::long_name () {
		/* Manages the long name.
		 * Return a node with a value.
		 */
		int start {getTokenStart()};
		eat(Token::LBRACE,Token::LNAME,Token::MAX_TYPES_NAMESPACE_VALUES); // eat { and get long name
		bst::str val {current_token->getValue()};
		eat(Token::LNAME,Token::MAX_TYPES_NAMESPACE_VALUES); // eat lname
		int end {getTokenStart()};
		eat(Token::RBRACE,Token::MAX_TYPES_NAMESPACE_VALUES); // eat }
		
		return Node{Node::LONG_NAME,Position(start,end,filepath,source),val};
	}
	

	Node Parser::options () {
		/* Manages all the options:
		 * lang, mark and arg 
		 * Return a Node with no value
		 * but fields named with options
		 * names.
		 */
		int start {getTokenStart()}; // start position
		std::array<bst::str,4> optionsnames {{"lang","mark","arg","return"}};
		NodeVector fields;
		for (int i{0};in<bst::str,std::array<bst::str,4>>(current_token->getValue(),optionsnames);++i) {
				if (current_token->getValue() == "lang") {
					for (const auto& child : lang_option())
						fields.push_back(child.second);
					optionsnames[0] = "";
				}

				else if (current_token->getValue() == "mark") {
					fields.push_back(simple_option(Token::NUMBER,Node::MARK));
					optionsnames[1] = "";
				}

				else if (current_token->getValue() == "arg") {
					for (const auto& child : param_option())
						fields.push_back(child);
					optionsnames[2] = "";
				}
				
				else if (current_token->getValue() == "return") {
					fields.push_back(simple_option(Token::VARIABLE,Node::RETURN_TYPE));
					optionsnames[3] = "";
				}

				eat(Token::RAQUOTE,Token::LAQUOTE,Token::ID,Token::MAX_TYPES_HEADER); // consume the end of each option: », and expects either « or and id
		}
		if (fields.size () == 0) 
			return Node(Node::EMPTY,Position(start,start,filepath,source));

		int end { fields.back().getPosition().getEnd() };// end position

		return Node(Node::OPTIONS,Position(start,end,filepath,source),fields);
	}

	Node Parser::simple_option (Token::Type tok_type, Node::Type node_type) {
		/* Manages a simple option with only one
		 * element inside
		 * tok_type is the type expected,
		 * node_type the type of the node returned
		 * Return a node with a value
		 * but no children
		 */
		int start {getTokenStart()};
		eat(Token::ID,Token::LAQUOTE,Token::MAX_TYPES_HEADER); // consume "mark"/"return"
		eat(Token::LAQUOTE,tok_type,Token::MAX_TYPES_VALUES); // consume «
		bst::str value { current_token->getValue()};
		eat(tok_type,Token::RAQUOTE,Token::MAX_TYPES_HEADER); // consume number/return type name
		int end {current_token->getPosition().getEnd()};
		return Node(node_type,Position(start,end,filepath,source),value);
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
		 * number of args, 
		 * choices 
		 */
		int start{getTokenStart()};
		StrNodeMap fields;
		fields.insert({"variable" , variable_declaration()});
		if (current_token->getType() == Token::COMMA) {
			eat(Token::COMMA,Token::MAX_TYPES_VALUES); // eat ,
			if (intokl(current_token->getType(),{Token::EQUAL,Token::LESS,Token::GREATER})){ // number of args
				NodeVector argnb {arg_number()};
				for (unsigned int i{0}; i < argnb.size();++i)
					fields.insert({bst::str("argnb") + i,argnb.at(i)});

				if (current_token->getType() == Token::COMMA)
					eat(Token::COMMA,Token::MAX_TYPES_VALUES); // eat ,
			}
			
			if (current_token->getType() == Token::LBRACKET) // list
				fields.insert({"choices",list()});
		}

		int end{current_token->getPosition().getStart() -1};

		return Node(Node::PARAM,Position(start,end,filepath,source),fields);
	}

	NodeVector Parser::arg_number () {
		/* Manages the number of args.
		 * Return a NodeVector with at max
		 * two fields
		 */

		NodeVector fields;
		while (intokl(current_token->getType(),{Token::EQUAL,Token::GREATER,Token::LESS})) {
			int start{getTokenStart()};
			bst::str val { current_token->getValue()};
			eat(current_token->getType(),Token::MAX_TYPES_VALUES); // eat =,> or <
			if ((val == ">" || val == "<") && current_token->getType() == Token::EQUAL) {
				val += "=";
				eat(Token::EQUAL,Token::MAX_TYPES_VALUES);
			}
			Node expression{expr()}; // get the number. We do not call comparison, since we don't expect one. Actually, calling comparison would create an error if another comparison sign is found
			int end {expression.getPosition().getEnd()};
			fields.push_back(Node{Node::BIN_OP,Position(start,end,filepath,source),{expression},val});
		}
		if (fields.size() > 2) {
			int start {fields.at(0).getPosition().getStart()};
			int end {fields.back().getPosition().getEnd()};
			throw BaseException(BaseException::SyntaxError,"It is impossible to specify more than two limits for the number of arguments.",Position(start,end,filepath,source));
		}

		return fields;
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

	Node Parser::list() {
		/* Manages the list
		 * Return a Node list
		 * with the elements inside the list
		 */
		int start{getTokenStart()};
		NodeVector elements;
		eat(Token::LBRACKET,Token::MAX_TYPES_VALUES); // eat [
		while (current_token->getType() != Token::RBRACKET) {
			elements.push_back(comparison());
			if (current_token->getType() == Token::VBAR)
				eat(Token::VBAR,Token::MAX_TYPES_VALUES); // eat ¦
		}
		int end{getTokenStart()};
		eat(Token::RBRACKET,Token::MAX_TYPES_VALUES); // eat ]
		return Node{Node::LIST,Position(start,end,filepath,source),elements};
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
					/*children.insert(
							{i,Node{Node::SIMPLE_TEXT,current_token->getPosition(),current_token->getValue()}}
						       );
						       */
					transpiler->append(current_token->getValue());
					eat(Token::SIMPLE_TEXT,Token::MAX_TYPES_TEXT);
					break;
				case Token::TAG:
					tag();
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

	void Parser::tag() {
		/* Manages the Tag token
		 */
		// keep the tag
		std::unique_ptr<Position> pos{std::make_unique<Position>(current_token->getPosition())};
		bst::str tag{current_token->getValue()};
		eat(Token::TAG,Token::TAIL,Token::MAX_TYPES_TEXT);
		// append tail if necessary
		if (current_token->getType() == Token::TAIL) {
			tag += current_token->getValue();
			pos = std::make_unique<Position>(*pos + current_token->getPosition());
			eat(Token::TAIL,Token::MAX_TYPES_TEXT);
		}
		// get the symbol
		Symbol& s{header_symbol->getChildren().find(tag)};
		// call
		try {
		transpiler->append(s.call(*this));
		} catch (const BackException& e) {
			// return type error
			throw iexc(e.type,"Symbol called does not return text: ", s.getReturnTypePos(),"Symbol called here: ",*pos);
#pragma message "Do not forget to catch lang error"
		}
	}

	std::map<bst::str,bst::str>Parser::files {};

} // namespace
