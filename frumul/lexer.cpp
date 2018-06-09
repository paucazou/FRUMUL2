#include "lexer.h"
#include <iostream>

namespace frumul {
#ifdef DEBUG
	void Lexer::test () {
	bst::str s{"#ifNdef"};
		std::cout << "Recognized: " << (recognizeCaselessID(s)? "Yes":"No") << "\n";

	}
#endif

	Lexer::Lexer (bst::str& nsource):
		source{nsource}
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
		 */
		pos = npos;
		if (pos < source.uLength())
			current_char = source.uAt(pos);
		else // end of input
			current_char = "";
	}

	bool Lexer::recognizeCaselessID (bst::str& candidate) {
		/* True if candidate can be found in source
		 * at pos position.
		 */
		tempos = pos;
		for (int cpos{0};
				cpos < candidate.uLength() && 
				tempos < source.uLength();
				++cpos, ++tempos
		    ) {
			bst::str c{source.uAt(tempos)};
			c.tolower();
			bst::str d{candidate.uAt(cpos)};
			d.tolower();
			if (c != d)
				return false;
		}
		return true;
	}
}// namespace
