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
			Lexer (const bst::str& nsource, const bst::str& nfilepath);
			template <typename ...T>
			Token getNextToken(T ...expected);
			void test();
		private:
			//attributes
			const bst::str& source;
			const bst::str& filepath;
			int pos{0}; // used to get the current position in the source
			int tempos{0}; // used to get temporary look further
			int line{1}; // current line and column
			int column{1};

			bst::str current_char; 
			cpUcs4 raw_current_char;
			// static attributes
			static const bst::str unbreakable_space;
			//functions
			Token _getNextToken(std::initializer_list<Token::Type> expected);
			void advanceBy (int step=1); 
			void advanceTo (int npos);
			bool skipComment (); // test and skip comments
			bool skipWhiteSpace (); 
			bool skipNoToken ();
			bst::str escape ();
			bool recognizeCaselessID (const bst::str& candidate);
			Token getID ();
			Token tokenizeBasicValue();
	};
}//namespace

#include "functions.inl"
// aliasing some template functions
namespace frumul {
	const auto toklists {&intersection<std::initializer_list<Token::Type>>};
	const auto intokl {&in<Token::Type,std::initializer_list<Token::Type>>};
}
#endif
