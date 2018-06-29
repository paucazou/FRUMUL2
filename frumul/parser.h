#ifndef PARSER_H
#define PARSER_H

#include <map>
#include "lexer.h"
#include "node.h"
#include "token.h"

namespace frumul {
	class Parser {
		/* Parser of the document.
		 * It creates
		 * the AST
		 */
		public:
			Parser (const bst::str& nsource,const bst::str& nfilepath);
			~Parser();
			Node& parse (); 
		private:
			//attributes
			bool alreadyparsed{false};
			const bst::str& source;
			const bst::str& filepath;
			Lexer lex;
			Node AST;
			Token * current_token {nullptr};
			//member functions
			int getTokenStart () const;

			template <typename ...T>
				bool eat(Token::Type t, T ...expected);
			bool _eat(Token::Type t, std::initializer_list<Token::Type> expected);

			Node document ();

			Node header ();
			std::map<bst::str,Node> statement_list (bool isNamespace = false);
			Node declaration ();

			Node basic_value (int start);

			Node options ();
			Node mark_option ();
			std::map<bst::str,Node> lang_option ();

			Node text ();
	};
}// namespace



#endif
