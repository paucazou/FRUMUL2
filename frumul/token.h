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
				LAQUOTE,	// «
				RAQUOTE,	// »
				COLON,		// :
				KEYWORD,	// file, namespace, alias, arg, lang, mark
				ID,		// everything else, followed by whitespace(s)
				TEXT, 		// ___text___
				MAX_TYPES_HEADER,
				// inside values
				LBRACE,		// {
				RBRACE,		// }
				LBRACKET,	// [
				RBRACKET,	// ]
				COMMA,		// ,
				VBAR,		// ¦
				EQUAL,		// =
				GREATER,	// >
				LESS,		// <
				NUMBER,		// 1,2,3...
				ANTISLASH,	/* \  */
				PARENT,		// §
				PLUS,		// +
				MINUS,		// -
				MUL,		// *
				DIV,		// /
				MODULO,		// %
				AND,		// &
				OR,		// |
				NOT,		// !
				VAL_TEXT,	// everything else
				MAX_TYPES_VALUES,
				// inside text
				TAG,		// opening or closing tag
				TAIL,		// opening tag tail
				SIMPLE_TEXT,	// everything else
				MAX_TYPES_TEXT,
			};
			//constructors
#ifdef DEBUG
			Token ();
#endif
			Token (Type ntype, const bst::str& nvalue, const Position& pos);
			Token (Type ntype, const bst::str& nvalue,
					int c1, int l1, int c2, int l2,
				       	const bst::str& filepath, const bst::str& filecontent);
			Token (Type ntype, const bst::str& nvalue,
					const Point& start, const Point& end,
				       	const bst::str& filepath, const bst::str& filecontent);
			//getters
			const Position& getPosition() const;
			Type getType() const;
			const bst::str& getValue() const;
			//overload
			friend std::ostream& operator << (std::ostream& out, const Token& tok);
			//other functions
			bst::str toString() const;
			static bst::str typeToString (Type t);

		private:
			const Type type;
			const bst::str value;
			const Position position;

	};
}// namespace
#endif
