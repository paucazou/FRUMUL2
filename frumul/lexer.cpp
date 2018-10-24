#include "lexer.h"
#include <iostream>
#include <locale>

// TODO end of file is not well handled


namespace frumul {
#ifdef DEBUG
#define printline std::cout << __LINE__ << std::endl
	void Lexer::test () {
		recognizeCaselessID("___header___");
		advanceTo(tempos);
		printl(skipNoToken());
		printl(Position(pos,pos,filepath,source));

	}
#endif
	// class members
	// public functions

	Lexer::Lexer (const bst::str& nsource, const bst::str& nfilepath):
		source{nsource}, filepath{nfilepath}, current_char{nsource.uAt(0)}, raw_current_char{nsource.uRawAt(0)}
	{}

	void Lexer::setOpeningTags(const std::vector<bst::str>& new_opening_tags) {
		/* Set the opening tags
		 */
		if (!new_opening_tags.empty())
			opening_tags = new_opening_tags;
	}



	Token Lexer::getNextToken (std::initializer_list<Token::Type> expected) {
		/* Return next token, following what
		 * is expected. The order of the arguments
		 * is important: it is the order
		 * of importance.
		 */
		if (current_char == "")
			return Token(Token::EOFILE,current_char,Position(pos,pos,filepath,source));

		if (intokl (Token::MAX_TYPES_LANG_VALUES,expected))
			return tokenizeLangValue(expected);

		if (intokl (Token::MAX_TYPES_NAMESPACE_VALUES,expected))
			return tokenizeNamespaceValue(expected);

		if (intokl (Token::MAX_TYPES_HEADER, expected)) {
			// inside header
			skipNoToken();

			if (recognizeCaselessID("___header___")) {
				const auto tok{Token(Token::HEADER,"___header___",
					Position(pos,tempos-1,filepath,source))};
				advanceTo(tempos);
				return tok;

			} else if (recognizeCaselessID("___text___")) {
				const auto tok {Token(Token::TEXT,"___text___",
						Position(pos,tempos-1,filepath,source))};
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
					advanceTo(oldpos);
					return tok;
				}
				if (current_char != ":") {
					advanceTo(oldpos);
					return tok;
				}
				advanceBy();
				if (! skipWhiteSpace()) {
					advanceTo(oldpos);
					return tok;
				}
				skipNoToken();
				if (current_char != ":") {
					advanceBy(oldpos);
					return tok;
				}
			}
			if (intokl(Token::LAQUOTE,expected) && current_char == "«") {
				// it is possible that LAQUOTE is requested, 
				// and ID too, but LAQUOTE should have precedence
				Token t {Token::LAQUOTE,"«",Position(pos,pos,filepath,source)};
				advanceBy();
				return t;
			}
			// no else here, because previous case may have found an ID is required
			if (intokl(Token::ID,expected))
				return getID();

			else if (intokl(Token::KEYWORD,expected))
				return getID(true); // true: to lowercase the value

			else {
				// values to return
				bst::str val;
				Token::Type t{Token::MAX_TYPES_VALUES}; // initial value to silent -Wsometimes-unitialized
				if (current_char == "(") {
					val = ")";
					t = Token::LPAREN;
				} else if (current_char == "»") {
					val = "»";
					t = Token::RAQUOTE;
				} else if (current_char == ":") {
					val = ":";
					t = Token::COLON;
				} else
					throw createUnexpectedToken(expected);
				advanceBy();
				return Token(t,val,
						Position(pos-1,pos-1,filepath,source));
				

			}
			// if no token was recognized 
			throw createUnexpectedToken(expected);
		}
		if (intokl(Token::MAX_TYPES_VALUES,expected))
			return tokenizeValue(expected);

		if (intokl(Token::MAX_TYPES_TEXT,expected)) {
			// tokenize the text 
			bst::str val;
			int oldpos {pos};

			// tail
			if (intokl(Token::TAIL,expected)) {
				while ( !iswspace(raw_current_char) && current_char != unbreakable_space && current_char != "") {
					val += current_char;
					advanceBy();
				}
				advanceBy(); // skip the next space
				if (val) // return a token only if necessary
					return Token(Token::TAIL,val,Position(oldpos,pos-1,filepath,source));
			}
			// simple text
			while (!isStartOfTag() && current_char != "") {
				val += current_char;
				advanceBy();
			}
			if (val) // return token if necessary
				return Token(Token::SIMPLE_TEXT,val,Position(oldpos,pos-1,filepath,source));
			// tag
			return findOpeningTag();
		}

		return Token();
	}

	Token Lexer::peekToken (std::initializer_list<Token::Type> expected, unsigned int rank) {
		/* without changing position,
		 * find the token at the position
		 * matching with rank. rank 0 = next token
		 * Even if rank indicates a token out
		 * of the boundaries of the source,
		 * it return the EOFILE.
		 */

		// save the infos
		int pos_saved{pos};

		try {
			for (; rank >= 1; --rank) // rank is an unsigned int, since it should not be under zero
				getNextToken(expected);
			Token returned {getNextToken(expected)}; // Token is const, so it should

			// reload the infos
			advanceTo(pos_saved);

			return returned;

		} catch (exc& e) {
			return Token();
		}
	}
	
	// private functions
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
		if (npos < source.uLength()) {
			current_char = source.uAt(npos);
			raw_current_char = source.uRawAt(npos);
		}
		else {// end of input 
			current_char = "";
			raw_current_char = 0;
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
			int start{pos};
			advanceTo(tempos);
			int end{pos-1};
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
		 * BUG TODO new line is not handled actually
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
		auto raw_char_to_change{raw_current_char};
		auto char_to_change{current_char};
		advanceBy();
		switch (raw_char_to_change) {
			case lquote:
			case rquote:
			case lbrace:
			case rbrace:
			case slash:
			case antislash:
				return char_to_change;
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
	Token Lexer::getID(Token::Type t, bool kwExpected) { 
		/* Recognize an ID
		 * and return the token.
		 * An ID starts and ends with whitespace.
		 * t is the type to return (usually Token::ID)
		 * if kwExpected is true (default is false)
		 * , then the value is lowercased.
		 */
		//std::setlocale(LC_ALL,std::locale("").name().data()); WARNING do not forget to set locale
		bst::str value;
		int start{pos};
		while (!std::iswspace(raw_current_char) && current_char != unbreakable_space && current_char != "") {
			value += current_char;
			advanceBy();
		}
		int end{pos-1};
		if (kwExpected)
			value.tolower();
		return Token{t,value,Position(start,end,filepath,source)};

	}

	Token Lexer::getID(bool kwExpected) {
		/* Simple wrapper of getID(Token::Type,bool)
		 */
		return getID(Token::ID,kwExpected);
	}

	Token Lexer::tokenizeNamespaceValue (std::initializer_list<Token::Type> expected) {
		/* Lexicalize namespace values
		 */
		bst::str val;
		int oldpos{pos};
		Token::Type t;
		skipNoToken();

		if (intokl(Token::LNAME,expected)) {
			// long name
			t = Token::LNAME;
			while (current_char != "}" && current_char != "") {
				if (current_char == "\\")
					val += escape();
				else if (current_char == "/") {
					skipComment();
					continue;
				}
				else if (current_char == "\n" || current_char == "\t") {
					advanceBy();
					continue;
				}
				else
					val += current_char;
				advanceBy();
			}
		}

		else if (current_char == "{") {
			t = Token::LBRACE;
			val = "{";
			advanceBy();
		} else if (current_char == "}") {
			t = Token::RBRACE;
			val = "}";
			advanceBy();
		}
		else if (current_char == "»") {
			t = Token::RAQUOTE;
			val = "»";
			advanceBy();
		}

		else {
			// short name
			t = Token::SNAME;
			if (current_char == "\\") {
				val = escape();
			}
			else
				val = current_char;
			advanceBy();
		}
		return Token(t,val,Position(oldpos,pos-1,filepath,source));
	}

	Token Lexer::tokenizeLangValue (std::initializer_list<Token::Type> expected) {
		/* Lexicalize inside lang values
		 */
		bst::str val;
		int oldpos{pos};
		Token::Type t;
		if (intokl(Token::LANGNAME,expected)) {
			t = Token::LANGNAME;
			while (current_char != "¦" && current_char != "»") {
				val += current_char;
				advanceBy();
			}
			if (!val)
				throw BaseException(BaseException::UnexpectedToken,"No language name entered.",Position(pos,pos,filepath,source));
		}
		else {
			if (current_char == "¦") {
				val = "¦";
				t = Token::VBAR;
			} else if (current_char == "»") {
				val = "»";
				t = Token::RAQUOTE;
			}
			else
				throw createUnexpectedToken(expected);
			advanceBy();
		}

		return Token(t,val,Position(oldpos,pos-1,filepath,source));
	}

	Token Lexer::tokenizeValue (std::initializer_list<Token::Type> expected) {
		/* Lexicalize inside values
		 */


		// Right quote, end of the value or of a litteral text
		if (current_char == "»") {
			advanceBy();
			return Token(Token::RAQUOTE,"»",Position(pos-1,pos-1,filepath,source));
		}

		// Left brace, start of programmatic part
		if (current_char == "{") {
			advanceBy();
			return Token {Token::LBRACE,"{",Position(pos-1,pos-1,filepath,source)};
		}
		// basic case : regular string, and litteral text
		if (toklists({Token::VAL_TEXT,Token::LITTEXT},expected)) {
			bst::str val;
			int start = pos;
			while (current_char != "»" && current_char != "") {
				if (current_char == "\\")
					val += escape();
				else if (current_char == "{" && !intokl(Token::LITTEXT,expected))
					break; // in this case, the lexer must break because a programmatic part is discovered
				else if (current_char == "/") {
					if (skipComment())
						continue;
				}
				else if (current_char == "\n" || current_char == "\t") {
					advanceBy();
					continue;
				}
				else if (current_char == "") {
					throw BaseException(BaseException::SyntaxError,"Unfinished value.",
							Position(start, pos-1,filepath,source));
				}
				else {
				val += current_char;
				advanceBy();
				}
			}
		// type is the first one expected, either VAL_TEXT or LITTEXT
		return Token {*expected.begin(),val,Position(start,pos-1,filepath,source)};
		}

		skipNoToken(); // whitespaces and comments must be deleted inside programmatic parts
		// rbrace: end of programmatic part
		if (current_char == "}") {
			advanceBy();
			return Token {Token::RBRACE,"}",Position(pos-1,pos-1,filepath,source)};
		}

		// programmatic parts
		if (std::iswdigit(raw_current_char)) {
			// manages numbers
			bst::str val;
			int oldpos{pos};
			while (iswdigit(raw_current_char)) {
				val += current_char;
				advanceBy();
			}
			return Token(Token::NUMBER,val,Position(oldpos,pos-1,filepath,source));
		}
		if (std::iswalnum(raw_current_char) || current_char == "_") {
			bst::str val;
			int oldpos{pos};
			while (iswalnum(raw_current_char) || current_char == "_") {
				val += current_char;
				advanceBy();
			}
			return Token{Token::VARIABLE,val,Position(oldpos,pos-1,filepath,source)};
		}
		bst::str val;
		Token::Type t;
		if (current_char == "[") {
			val = "[";
			t = Token::LBRACKET;
		}
		else if (current_char == "]") {
			val = "]";
			t = Token::RBRACKET;
		}
		else if (current_char == ")") {
			val = ")";
			t = Token::RPAREN;
		}
		else if (current_char == "(") {
			val = "(";
			t = Token::LPAREN;
		}
		else if (current_char == ",") {
			val = ",";
			t = Token::COMMA;
		}
		else if (current_char == "¦") {
			val = "¦";
			t = Token::VBAR;
		}
		else if (current_char == ":") {
			val = ":";
			t = Token::ASSIGN;
		}
		else if (current_char == "=") {
			val = "=";
			t = Token::EQUAL;
		}
		else if (current_char == ">") {
			val = ">";
			t = Token::GREATER;
		}
		else if (current_char == "<") {
			val = "<";
			t = Token::LESS;
		}
		else if (current_char == "§") {
			return getID(Token::SYMBOL);
		}
		else if (current_char == "+") {
			val = "+";
			t = Token::PLUS;
		}
		else if (current_char == "-") {
			val = "-";
			t = Token::MINUS;
		}
		else if (current_char == "*") {
			val = "*";
			t = Token::MUL;
		}
		else if (current_char == "/") {
			val = "/";
			t = Token::DIV;
		}
		else if (current_char == "%") {
			val = "%";
			t = Token::MODULO;
		}
		else if (current_char == "&") {
			val = "&";
			t = Token::AND;
		}
		else if (current_char == "|") {
			val = "|";
			t = Token::OR;
		}
		else if (current_char == "!") {
			val = "!";
			t = Token::NOT;
		}
		else if (current_char == "«") {
			val = "«";
			t = Token::LAQUOTE;
		}
		else
			throw createUnexpectedToken(expected);
		advanceBy();
		return Token(t,val,Position(pos-1,pos-1,filepath,source));

	}
	bool Lexer::isStartOfTag() {
		/* Try to see if the current char is the start of
		 * at least one tag
		 */
		int remaining_length { source.uLength() - pos};
		for (const auto & tag : opening_tags) {
			int taglen {tag.uLength()};
			if (remaining_length >= taglen) {
				if (source.uRange(pos,pos + taglen-1) == tag)
					return true;
			}
		}
		return false;
	}

	Token Lexer::findOpeningTag() {
		/* Find the correct opening tag
		 */
		int remaining_length { source.uLength() - pos };
		int oldpos{pos};
		bst::str chosen;
		for (const auto& tag : opening_tags) {
			int taglen {tag.uLength()};
			if (remaining_length >= taglen)
				if (source.uRange(pos,pos + taglen - 1) == tag && taglen > chosen.uLength())
					chosen = tag;
		}
		if (!chosen) {
			bst::str msg{"An opening tag was expected but wasn't found.\nHere is the list of the tags found in the header:\n"};
			for (const auto& tag : opening_tags)
				msg += tag + "\n";
			throw BaseException(BaseException::TagNotFound,msg,Position(pos,pos,filepath,source));
		}

		advanceTo(pos+chosen.uLength());
		return Token(Token::TAG,chosen,Position(oldpos,pos-1,filepath,source));
	}

	BaseException Lexer::createUnexpectedToken(std::initializer_list<Token::Type> expected) {
			/* Instanciates an UnexpectedToken exception
			 */
			bst::str tokensexpected{"Following tokens were expected:\n"};
			for (const auto & tok : expected)
				tokensexpected += Token::typeToString(tok) + "\n";
			return BaseException(BaseException::UnexpectedToken,tokensexpected,
					Position(pos,pos,filepath,source));
			}

	const bst::str Lexer::unbreakable_space{L'\u00A0'}; // unbreakable space isn't considered a space
}// namespace
