#ifndef TOKEN_H
#define TOKEN_H
#include "macros.h"
#include "position.h"
//#include "header.h"
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
	TOK(SEMICOLON)		/* ; */\
	TOK(VBAR)		/* ¦ */\
	TOK(ASSIGN)		/* : */\
	TOK(EQUAL)		/* = */\
	TOK(GREATER)		/* > */\
	TOK(LESS)		/* < */\
	TOK(NUMBER)		/* 1,2,3... */\
	TOK(SYMBOL)		/* § and the tail */\
	TOK(PLUS)		/* + */\
	TOK(MINUS)		/* - */\
	TOK(MUL)		/* * */\
	TOK(DIV)		/* / */\
	TOK(MODULO)		/* % */\
	TOK(AND)		/* & */\
	TOK(OR)			/* | */\
	TOK(NOT)		/* ! */\
        TOK(DOT)                /* . */\
	TOK(VARIABLE)		/* alphanumeric + _ name */\
	TOK(LITTEXT)		/* litteral text */\
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
			//constructors
			Token ();
			Token (Type ntype, const FString& nvalue, const Position& pos);
			Token (Type ntype, const FString& nvalue,
					int c1, int l1, int c2, int l2,
				       	const FString& filepath, const FString& filecontent);
			Token (Type ntype, const FString& nvalue,
					const Point& start, const Point& end,
				       	const FString& filepath, const FString& filecontent);
			//getters
			const Position& getPosition() const;
			Type getType() const;
			const FString& getValue() const;
			//overload
			friend std::ostream& operator << (std::ostream& out, const Token& tok);
			//other functions
			FString toString() const;

		private:
			const Type type;
			const FString value;
			const Position position;

	};
}// namespace
#endif
