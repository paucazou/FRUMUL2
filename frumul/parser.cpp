#include "functions.inl"
#include "parser.h"

namespace frumul {
 	// constructors
	Parser::Parser (const bst::str& nsource, const bst::str& nfilepath) :
		source{nsource}, filepath{nfilepath},
		lex{nsource,nfilepath},
		AST{Node::DOCUMENT,Position{0,nsource.uLength()-1,nfilepath,source}}
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
	
	int getTokenStart () const {
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
		if (current_token->getType() == t) {
			delete current_token;
			current_token = new Token{lex.getNextToken(expected)};
			return true;
		}
		throw BaseException(BaseException::UnexpectedToken,"Token expected: "+Token::typeToString(t),Position(current_token->getPosition()));
		return false;
	}

	// parser itself
	
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

			if (!namespace)
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

		if (current_token->getType() == Token::LAQUOTE) { // we can assume it is a basic value
			int start {getTokenStart()};
			eat(Token::LAQUOTE,Token::VAL_TEXT,Token::LBRACE,Token::MAX_TYPES_VALUES);
			fields.insert({"value",basic_value(start)});
			//RAQUOTE is eat in statement_list
		}

		int end {fields["value"].getPosition().getEnd()}; // end position


		return Node{Node::DECLARATION,Position(start,end,filepath,source),fields,name};
	}

	Node Parser::basic_value (int start) {
		/* Manages basic value
		 * Return a node with the basic value
		 * Node has no value, but the fields are
		 * filled with nodes ordered by digits
		 * start is the position of the LAQUOTE
		 * TODO not finished
		 */
		std::vector<Node> fields;
		while (current_token->getType() != Token::RAQUOTE) {
			// simple text
			if (current_token->getType() == Token::VAL_TEXT) {
				fields.push_back(Node(Node::VAL_TEXT,current_token->getPosition(),{},current_token->getValue()));
				eat(Token::VAL_TEXT,Token::LBRACE,Token::MAX_TYPES_VALUES);
			}
		}
		int end {current_token->getPosition().getEnd()}; // should match with the position of RAQUOTE
		return Node(Node::BASIC_VALUE,Position(start,end,filepath,source),fields);

	}

	Node Parser::options () {
		/* Manages all the options:
		 * lang, mark and arg (TODO)
		 * Return a Node with no value
		 * but fields named with options
		 * names.
		 */
		int start {getTokenStart()}; // start position
		std::array<bst::str> optionsnames {"lang","mark","arg"};
		std::map<bst::str,Node> fields;
		for (int i{0};in<bst::str,std::array<bst::str>>(current_token->getValue(),optionsnames);++i) {
				if (current_token->getValue() == "lang") {
					for (const auto& child : lang_option();)
						fields[i] = child.second;
					optionsnames[0] = "";
				}
				else if (current_token->getValue() == "mark") {
					fields[i] = mark_option ();
					optionsnames[1] = "";
				}

				eat(Token::RAQUOTE,Token::LAQUOTE,Token::ID,Token::MAX_TYPES_HEADER); // consume the end of each option: », and expects either « or and id
		}
		int end { fields.rbegin()->second.getPosition().getEnd() }// end position

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
		int end {current_token->getPosition().getEnd()};
		return Node(Node::MARK,Position(start,end,filepath,source),{},value);
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
		while (current_token->getType() != Token::RAQUOTE) {
			Node n {Node::LANG,Token->getPosition(),{},Token->getValue()};
			fields.insert{i,n};
			eat(Token::LANGNAME,Token::VBAR,Token::RAQUOTE,Token::MAX_TYPES_LANG_VALUES);
			if (current_token->getType() == Token::VBAR)
				eat(Token::VBAR,Token::LANGNAME,Token::MAX_TYPES_LANG_VALUES);
		}
		return fields;
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
