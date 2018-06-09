#ifndef LEXER_H
#define LEXER_H
//#include "parser.h" // not sure it's useful
#include <initializer_list>
#include "token.h"

namespace frumul {
	class Lexer {
		/* Tokenize the document
		 */
		public:
			Lexer (bst::str& nsource);
			template <typename ...T>
			Token getNextToken(T ...expected);
			void test();
		private:
			//attributes
			bst::str& source;
			int pos{0}; // used to get the current position in the source
			int tempos{0}; // used to get temporary look further
			bst::str current_char; 
			//functions
			Token _getNextToken(std::initializer_list<Token::Type> expected);
			void advanceBy (int step=1); 
			void advanceTo (int npos);
			bool recognizeCaselessID (bst::str& candidate);
	};
}//namespace

#endif
