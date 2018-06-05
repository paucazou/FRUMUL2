#ifndef TOKEN_H
#define TOKEN_H
#include "position.h"
namespace frumul {
	class Token {
		/* Handles the tokens found
		 * in the header and the text.
		 */
		public:
			enum Type {
				/* Contains all the types 
				 * which can be found
				 * in a FRUMUL file
				 */
				// inside header
				HEADER,		// ___header___
				LPAREN,		// (
				RPAREN, 	// )
				LBRACE,		// {
				RBRACE,		// }
				LBRACKET,	// [
				RBRACKET,	// ]
				LAQUOTE,	// «
				RAQUOTE,	// »
				COLON,		// :
				COMMA,		// ,
				VBAR,		// ¦
				EQUAL,		// =
				GREATER,	// >
				LESS,		// <
				NUMBER,		// 1,2,3...
				ANTISLASH,	// \ 
				PARENT,		// §
				PLUS,		// +
				MINUS,		// -
				MUL,		// *
				DIV,		// /
				MODULO,		// %
				AND,		// &
				OR,		// |
				NOT,		// !
				EOL,		// EOL, inside values
				TAB,		// TAB, inside values
				ID,		// everything else, enclosed by whitespaces
				// inside text
				TEXT,		// ___text___
				TAG,		// opening or closing tag
				TAIL,		// opening tag tail
				SIMPLE_TEXT,	// everything else
				MAX_TYPES,
			};
			//constructors
			Token (Type ntype, const bst::str& nvalue, const Position& pos);
			Token (Type ntype, const bst::str& nvalue, int column, int line, const bst::str& filepath, const bst::str& filecontent);
			//getters
			const Position& getPosition() const;
			const Type getType() const;
			const bst::str& getValue() const;
			//overload
			friend std::ostream& operator << (std::ostream& out, const Token& tok);
			//other functions
			bst::str toString() const;

		private:
			const Position position;
			const Type type;
			const bst::str value;

	};
}// namespace
#endif
