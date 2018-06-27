#ifndef TOKEN_H
#define TOKEN_H
#include "macros.h"
#include "position.h"
		/* */
#define TOKENS_LIST(TOK) \
	/* Contains all the types \
	 * which can be found \
	 * in a FRUMUL file \
	 */ \
	TOK(HEADER)		/* ___header___ */\
	TOK(LPAREN)		/* ( */\
	TOK(RPAREN)		/* ) */\
	TOK(LAQUOTE)		/* « */\
	TOK(RAQUOTE)		/* » */\
	TOK(COLON)		/* : */\
	TOK(KEYWORD)		/* file, namespace, alias, arg, lang, mark */\
	TOK(TEXT)		/* ___text___ */\
	TOK(ID)			/* everything else*/\
	TOK(MAX_TYPES_HEADER)\
	/* inside values */ \
	TOK(LBRACE)		/* { */\
	TOK(RBRACE)		/* } */\
	TOK(LBRACKET)		/* [ */\
	TOK(RBRACKET)		/* ] */\
	TOK(COMMA)		/* , */\
	TOK(VBAR)		/* ¦ */\
	TOK(EQUAL)		/* = */\
	TOK(GREATER)		/* > */\
	TOK(LESS)		/* < */\
	TOK(NUMBER)		/* 1,2,3... */\
	TOK(PARENT)		/* § */\
	TOK(PLUS)		/* + */\
	TOK(MINUS)		/* - */\
	TOK(MUL)		/* * */\
	TOK(DIV)		/* / */\
	TOK(MODULO)		/* % */\
	TOK(AND)		/* & */\
	TOK(OR)			/* | */\
	TOK(NOT)		/* ! */\
	TOK(VARIABLE)		/* alphanumeric + _ name */\
	TOK(VAL_TEXT)		/* everything else */\
	TOK(MAX_TYPES_VALUES)\
	/* special namespaces types */ \
	TOK(LNAME)		/* long name */\
	TOK(SNAME)		/* short name */\
	TOK(MAX_TYPES_NAMESPACE_VALUES)\
	/* special lang types */\
	TOK(LANGNAME)		/* name of a language */\
	TOK(MAX_TYPES_LANG_VALUES)\
	/* inside text */ \
	TOK(TAG)		/* opening or closing tag */\
	TOK(TAIL)		/* opening tag tail */\
	TOK(SIMPLE_TEXT)	/* everything else */\
	TOK(MAX_TYPES_TEXT)\
	TOK(EOFILE)		/* End Of File */\
	

namespace frumul {
	class Token {
		/* Handles the tokens found
		 * in the header and the text.
		 */
		public:
			ENUM(Type,TOKENS_LIST)
#if 0
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
				PARENT,		// §
				PLUS,		// +
				MINUS,		// -
				MUL,		// *
				DIV,		// /
				MODULO,		// %
				AND,		// &
				OR,		// |
				NOT,		// !
				VARIABLE,	// alphanumeric + _ name
				VAL_TEXT,	// everything else
				MAX_TYPES_VALUES,
				// special namespaces types
				LNAME,		// long name
				SNAME,		// short name
				MAX_TYPES_NAMESPACE_VALUES,
				// special lang types
				LANGNAME,	// name of a language
				MAX_TYPES_LANG_VALUES,
				// inside text
				TAG,		// opening or closing tag
				TAIL,		// opening tag tail
				SIMPLE_TEXT,	// everything else
				MAX_TYPES_TEXT,
				EOFILE,		// End Of File
			};
#endif
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

		private:
			const Type type;
			const bst::str value;
			const Position position;

	};
}// namespace
#endif
