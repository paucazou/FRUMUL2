#ifndef LEXER_H
#define LEXER_H
//#include "parser.h" // not sure it's useful
#include <initializer_list>
#include <vector>
#include "exception.h"
#include "token.h"
//#include "header.h"

namespace frumul {

	class Lexer {
		/* Tokenize the document
		 */
		public:
			Lexer (const FString& nsource, const FString& nfilepath);
			void setOpeningTags(const std::vector<FString>& new_opening_tags);
			Token getNextToken(std::initializer_list<Token::Type> expected);

			Token peekToken (std::initializer_list<Token::Type> expected, unsigned int rank=0);
			template <typename ...T>
				Token getNextToken(T ...expected) {
					return getNextToken({expected...});
				}

			template <typename ...T> 
				Token peekToken(unsigned int rank, T ...expected) {
					return peekToken({expected...}, rank);
				}

			void test();
		private:
			//attributes
			const FString& source;
			const FString& filepath;
			int pos{0}; // used to get the current position in the source
			int tempos{0}; // used to get temporary look further
			//int line{1}; // current line and column
			//int column{1};

			FString current_char; 
			UnicodePoint raw_current_char;

			std::vector<FString> opening_tags {}; // contains every opening tag discovered
			// static attributes
			static const FString unbreakable_space;
			//functions
			void advanceBy (int step=1); 
			void advanceTo (int npos);
			bool skipComment (); // test and skip comments
			bool skipWhiteSpace (); 
			bool skipNoToken ();
			FString escape ();
			bool recognizeCaselessID (const FString& candidate);
			Token getID (Token::Type t = Token::ID, bool kwExpected = false);
			Token getID (bool kwExpected);
			Token tokenizeNamespaceValue(std::initializer_list<Token::Type> expected);
			Token tokenizeLangValue (std::initializer_list<Token::Type> expected);
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
