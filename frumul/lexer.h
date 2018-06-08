#ifndef LEXER_H
#define LEXER_H
#include "parser.h"

namespace frumul {
	class Lexer {
		/* Tokenize the document
		 */
		public:
			Lexer (const bst::str& nsource);
		private:
			//attributes
			const bst::str& source;
	};
}//namespace

#endif
