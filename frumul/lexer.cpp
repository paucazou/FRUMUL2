#include "lexer.h"
#include "exception.h"
#include <iostream>
#include <locale>


namespace frumul {
#ifdef DEBUG
#define printline std::cout << __LINE__ << std::endl
	void Lexer::test () {
	bst::str s{"___header___"};
		std::cout << "Recognized: " << (recognizeCaselessID(s)? "Yes":"No") << "\n";
		Token tok{getID()};
		advanceBy();
		std::cout << tok.getValue() << "\n";
		std::cout << tok;
		bst::str chars[] {"«","»","{","}","n","t","/","\\"};
		for (int i{0}; i < 8; ++i)
			std::cout << chars[i]  << ": " << chars[i].uRawAt(0)<< std::endl;
		skipComment();
		std::cout << pos << "\n";
		std::cout << current_char << " - " << raw_current_char;
		std::cout << line << " c " << column << std::endl;
		std::cout<<escape()<< "\n";



	}
#endif
	// class members

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
			if (lnumber > 1) { 
				column = 0; 
				for (int i{range.uLength() -1}; range.uAt(i) != "\n"; --i)
					++column;
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
		tempos = pos;
	}

	bool Lexer::skipComment () {
		/* Tests if it is a comment
		 * and skip it.
		 * Must be called each time a '/' is found
		 * Return true if is a comment.
		 */
		// Test
		if (pos+2 >= source.uLength() || (source.uAt(pos+1) != "/" && source.uAt(pos+2) != "*"))
			return false;

		try {
			while (!(source.uAt(tempos) == "*" && source.uAt(tempos+1) == "/" && source.uAt(tempos+2) == "/"))
				++tempos;
		} catch (bst::CBStringException) {
			Point start{column,line};
			advanceTo(tempos);
			Point end{column,line};
			throw BaseException(BaseException::SyntaxError,"Unfinished comment.",
					Position{start,end, filepath, source});
		}
		advanceTo (tempos +3);
		return true;
	}

	void Lexer::skipWhiteSpace () {
		/* Skip whitespaces
		 * including unbreakable spaces
		 */
		while (std::iswspace(raw_current_char) || current_char == unbreakable_space )
			advanceBy();
	}

	bst::str Lexer::escape () {
		/* Manages all escape characters
		 */
		// returned unmodified
		static const cpUcs4 lquote {171}; // «
		static const cpUcs4 rquote {187}; // »
		static const cpUcs4 lbrace {123}; // {
		static const cpUcs4 rbrace {125}; // }
		static const cpUcs4 slash {47}; // /
		static const cpUcs4 antislash {92}; // \ /
		// returned modified
		static const cpUcs4 newline {110}; // \n
		static const cpUcs4 tab {116}; // \t
		advanceBy();
		switch (raw_current_char) {
			case lquote:
			case rquote:
			case lbrace:
			case rbrace:
			case slash:
			case antislash:
				return current_char;
			case newline:
				return "\n";
			case tab:
				return "\t";
			default:
				Position errorpos {pos-1,pos,filepath,source};
				throw BaseException(BaseException::SyntaxError,
						bst::str{"Unrecognized espaced sequence: \\"} + current_char,
						errorpos);

		}
	}

	bool Lexer::recognizeCaselessID (const bst::str& candidate) {
		/* True if candidate can be found in source
		 * at pos position.
		 * candidate must be an ASCII string.
		 */
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
		bst::str value;
		Point start{column,line};
		while (!std::iswspace(raw_current_char) && current_char != unbreakable_space && current_char != "") {
			value += current_char;
			advanceBy();
		}
		Point end{column-1,start.getLine()};
		return Token{Token::ID,value,start,end,filepath,source};

	}
	Token Lexer::tokenizeBasicValue () { // maybe it should take some parameters, some Token::Type expected
		bst::str val;
		int start = pos;
		// basic case : regular string
		while (current_char != "»") {
			if (current_char == "\\")
				val += escape();
			else if (current_char == "{")
				break; // in this case, the lexer must break because a programmatic part is discovered
			else if (current_char == "\n" || current_char == "\t")
				continue;
			else if (current_char == "") {
				throw BaseException(BaseException::SyntaxError,"Unfinished value.",
						Position(start, pos-1,filepath,source));
			} else
				val += current_char;
			advanceBy();
		}
		return Token {Token::VAL_TEXT,val,Position(start,pos-1,filepath,source)};

	}

	const bst::str Lexer::unbreakable_space{L'\u00A0'}; // unbreakable space isn't considered a space
}// namespace
