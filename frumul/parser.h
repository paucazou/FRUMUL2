#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "node.h"

namespace frumul {
	class Parser {
		/* Parser of the document.
		 * It creates
		 * the AST
		 */
		public:
			Parser (const bst::str& nsource);
			DocumentNode& parse (); 
		private:
			//attributes
			const bst::str source;
			Lexer lex;
			DocumentNode doc;
			//member functions
			DocumentNode document ();

			HeaderNode header ();
			StatementListNode statement_list ();

			TextNode text ();
	};
}// namespace



#endif
