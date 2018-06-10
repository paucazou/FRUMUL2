#include "lexer.h"
#include <iostream>
#include <locale>


namespace frumul {
#ifdef DEBUG
	void Lexer::test () {
	bst::str s{"#ifNdef"};
		std::cout << "Recognized: " << (recognizeCaselessID(s)? "Yes":"No") << "\n";
		Token tok{getID()};
		std::cout << tok.getValue() << "\n";
		std::cout << tok;
		std::cout << pos << "\n";
		std::cout << current_char << '-' << raw_current_char;


	}
#endif

	Lexer::Lexer (const bst::str& nsource, const bst::str& nfilepath):
		source{nsource}, filepath{nfilepath}, current_char{nsource.uAt(0)}, raw_current_char{nsource.uRawAt(0)}
	{}

	template <typename ...T>
		Token Lexer::getNextToken(T ...expected) {
			/* Pass the arguments to the real function
			 */
			return _getNextToken({expected...});
		}
	Token Lexer::_getNextToken (std::initializer_list<Token::Type> expected) {
		/* Return next token, following what
		 * is expected. The order of the arguments
		 * is important: it is the order
		 * of importance.
		 */
		return Token();
	}
	void Lexer::advanceBy (int step) {
		/* Change the position
		 * and the current_char
		 */
		advanceTo(pos+step);
	}

	void Lexer::advanceTo (int npos) {
		/* Change the position and the current char
		 * by advancing to npos
		 * Change column and line. 
		 */
		if (npos < source.uLength()) {
			current_char = source.uAt(npos);
			raw_current_char = source.uRawAt(npos);
			// columns and lines
			bst::str range {source.uRange(pos,npos)};
			int lnumber = range.lineNumber();
			if (lnumber > 1) { // TODO à vérifier
				for (int i{npos}; range.uAt(i) != "\n"; --i)

				column = 1 + npos - i; 
				line += lnumber-1;
			}
			else
				column = column + npos -pos;
		}
		else {// end of input 
			current_char = "";
			raw_current_char = 0;
			line = source.lineNumber();
			column = source.getLine(line).uLength();
		}
		pos = npos;
	}

	bool Lexer::recognizeCaselessID (const bst::str& candidate) {
		/* True if candidate can be found in source
		 * at pos position.
		 * candidate must be an ASCII string.
		 */
		tempos = pos;
		bst::str c, d;
		for (int cpos{0};
				cpos < candidate.uLength() && 
				tempos < source.uLength();
				++cpos, ++tempos
		    ) {
			c = source.uAt(tempos);
			c.tolower();
			d = candidate.uAt(cpos);
			d.tolower();
			if (c != d)
				return false;
		}
		return true;
	}
	Token Lexer::getID() { // maybe return the value only TODO
		/* Recognize an ID
		 * and return the token.
		 * An ID starts and ends with whitespace.
		 */
		//std::setlocale(LC_ALL,std::locale("").name().data()); WARNING do not forget to set locale
		static bst::str unbreakable_space{L'\u00A0'}; // unbreakable space isn't considered a space
		bst::str value;
		Point start{column,line};
		while (!std::iswspace(raw_current_char) && current_char != unbreakable_space && current_char != "") {
			value += current_char;
			advanceBy();
		}
		Point end{column-1,start.getLine()};
		return Token{Token::ID,value,start,end,filepath,source};

	}
}// namespace
