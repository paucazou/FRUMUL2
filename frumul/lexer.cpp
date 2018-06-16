#include "lexer.h"
#include "exception.h"
#include <iostream>
#include <locale>


namespace frumul {
#ifdef DEBUG
#define printline std::cout << __LINE__ << std::endl
#define printl(elt) std::cout << elt << std::endl
	void Lexer::test () {
		recognizeCaselessID("___header___");
		advanceTo(tempos);
		printl(skipNoToken());
		printl(Position(pos,pos,filepath,source));

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
		if (intokl (Token::MAX_TYPES_HEADER, expected)) {
			// inside header
			skipWhiteSpace();
			if (current_char == "/")
				skipComment();

			if (recognizeCaselessID("___header___")) {
				const auto tok{Token(Token::HEADER,"___header___",
					Position(pos,tempos,filepath,source))};
				advanceTo(tempos);
				return tok;

			} else if (recognizeCaselessID("___text___")) {
				const auto tok {Token(Token::HEADER,"___text___",
						Position(pos,tempos,filepath,source))};
				advanceTo(tempos);
				return tok;

			} else if (intokl(Token::ID,expected) && intokl(Token::RPAREN,expected) && current_char == ")") {
				// special case : is ')' the end of a namespace
				// or the start of an ID?
				// This part only returns the RPAREN token
				Token tok {Token::RPAREN,")",Position(pos,pos,filepath,source)}; // Token which may be returned if it is recognized
				advanceBy();
				bool isSkipped {skipNoToken()};
				int oldpos {pos};
				if (! isSkipped) {
					advanceBy(oldpos);
					return tok;
				}
				if (current_char != ":") {
					advanceBy(oldpos);
					return tok;
				}
				advanceBy();
				if (! skipWhiteSpace()) {
					advanceBy(oldpos);
					return tok;
				}
				skipNoToken();
				if (current_char != ":") {
					advanceBy(oldpos);
					return tok;
				}
			}
			// no else here, because previous case may have found an ID is required
			if (intokl(Token::ID,expected))
				return getID();

			else if (intokl(Token::KEYWORD,expected))
				return getID();

			else {
				// values to return
				bst::str val;
				Token::Type t;
				if (current_char == "(") {
					val = ")";
					t = Token::LPAREN;
				} else if (current_char == ")") {
					val = "(";
					t = Token::RPAREN;
				} else if (current_char == "«") {
					val = "«";
					t = Token::LAQUOTE;
				} else if (current_char == "»") {
					val = "»";
					t = Token::RAQUOTE;
				} else if (current_char == ":") {
					val = ":";
					t = Token::COLON;
				}
				advanceBy();
				return Token(t,val,
						Position(pos-1,pos-1,filepath,source));
				

			}
			// if no token was recognized TODO create a function to create the exception instance
			bst::str tokensexpected{"Following tokens were expected:\n"};
			for (const auto & tok : expected)
				tokensexpected += Token::typeToString(tok) + "\n";
			throw BaseException(BaseException::UnexpectedToken,tokensexpected,
					Position(pos,pos,filepath,source));
		}
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
		if (pos+2 >= source.uLength() || !(source.uAt(pos+1) == "/" && source.uAt(pos+2) == "*"))
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

	bool Lexer::skipWhiteSpace () {
		/* Skip whitespaces
		 * including unbreakable spaces
		 * true if spaces have been skipped
		 */
		int oldpos = pos;
		while (std::iswspace(raw_current_char) || current_char == unbreakable_space )
			advanceBy();
		return oldpos != pos;
	}
	bool Lexer::skipNoToken () {
		/* Skip comments and whitespaces
		 * return true if it has skipped something
		 */
		int oldpos = pos;
		while (skipWhiteSpace() || (current_char == "/" && skipComment()));

		return oldpos != pos;
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
		tempos = pos;
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
	Token Lexer::getID() { 
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
