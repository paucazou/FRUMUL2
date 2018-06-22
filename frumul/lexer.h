#ifndef LEXER_H
#define LEXER_H
//#include "parser.h" // not sure it's useful
#include <initializer_list>
#include <vector>
#include "exception.h"
#include "token.h"

namespace frumul {

	class Lexer {
		/* Tokenize the document
		 */
		public:
			Lexer (const bst::str& nsource, const bst::str& nfilepath);
			void setOpeningTags(const std::vector<bst::str>& new_opening_tags);
			Token getNextToken(std::initializer_list<Token::Type> expected);
			template <typename ...T>
				Token getNextToken(T ...expected) {
					return getNextToken({expected...});
				}
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

			std::vector<bst::str> opening_tags {}; // contains every opening tag discovered
			// static attributes
			static const bst::str unbreakable_space;
			//functions
			void advanceBy (int step=1); 
			void advanceTo (int npos);
			bool skipComment (); // test and skip comments
			bool skipWhiteSpace (); 
			bool skipNoToken ();
			bst::str escape ();
			bool recognizeCaselessID (const bst::str& candidate);
			Token getID ();
			Token tokenizeNamespaceValue(std::initializer_list<Token::Type> expected);
			Token tokenizeValue(std::initializer_list<Token::Type> expected);

			bool isStartOfTag();
			Token findOpeningTag();

			BaseException createUnexpectedToken(std::initializer_list<Token::Type> expected);
	};
}//namespace

#include "functions.inl"
// aliasing some template functions
namespace frumul {
	const auto toklists {&intersection<std::initializer_list<Token::Type>>};
	const auto intokl {&in<Token::Type,std::initializer_list<Token::Type>>};
}
#endif
