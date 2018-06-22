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
		 */
		AST.addChild("header",header());
		AST.addChild("text",text());
		return AST;
	}

	Node Parser::header() {
		/* manages the header of the document
		 * Return the header node
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

	std::map<bst::str,Node> Parser::statement_list() {
		/* Manages the list of declarations
		 */
		std::map<bst::str,Node> statements;
		if (current_token->getType() != Token::ID) {
			int nodepos {current_token->getPosition().getStart()};
			statements.insert({bst::str(0),Node(Node::EMPTY,Position(nodepos,nodepos,filepath,source))});
		}

		return statements;
	}

	Node Parser::text() {
		/* parses the text
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
							{i,Node{Node::SIMPLE_TEXT,current_token->getPosition()}}
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
