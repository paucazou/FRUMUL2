#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <memory>
#include "hinterpreter.h"
#include "lexer.h"
#include "node.h"
//#include "token.h"
#include "symbol.h"
#include "transpiler.h"
#include "util.h"

//#include "header.h"

namespace frumul {
	class Hinterpreter;
	class Transpiler;
	class Parser {
		/* Parser of the document.
		 * It creates
		 * the AST
		 */
		public:
			Parser (const bst::str& nsource,const bst::str& nfilepath,const Token::Type next_token=Token::MAX_TYPES_HEADER,Transpiler* ntranspiler=nullptr);
			Parser (const bst::str& nsource,const bst::str& nfilepath,Transpiler& ntranspiler);
			~Parser();
			Node& parse (); 
			const Symbol& getHeaderSymbol() const;
			const Token& getCurrentToken() const;
			template <typename ...T>
				bool eat(Token::Type t, T ...expected);
			const Transpiler& getTranspiler() const;
		private:
			//static attributes
			static std::map<bst::str,bst::str> files; // files already loaded
			//attributes
			bool alreadyparsed{false};
			const bst::str& source;
			const bst::str& filepath;
			std::shared_ptr<Symbol> header_symbol{nullptr};
			Lexer lex;
			Transpiler* transpiler{nullptr};
			Node AST;
			Token * current_token {nullptr};
			//member functions
			int getTokenStart () const;

			bool _eat(Token::Type t, std::initializer_list<Token::Type> expected);

			Node document ();

			Node header ();
			std::vector<Node> statement_list (bool isNamespace = false);
			Node declaration (const bool isNameRequired=true);

			Node basic_value (const int start);
			Node programmatic_part ();

			Node variable_declaration ();
			Node variable_assignment ();
			StrNodeMap types ();
			Node comparison ();
			Node expr ();
			Node term ();
			Node factor ();
			Node reference ();
			Node index ();

			Node loop ();
			Node condition ();

			Node path_value ();
			Node file_content (const Node& path_node);

			Node namespace_value (const int start);
			Node short_name ();
			Node long_name ();

			Node options ();
			Node simple_option(Token::Type, Node::Type);
			std::map<bst::str,Node> lang_option ();
			std::vector<Node> param_option ();
			Node param_value ();
			std::vector<Node> arg_number ();

			Node litteral ();
			Node list ();

			Node text ();
			void tag();

			Node left_bin_op(std::initializer_list<Token::Type>::iterator);

			template <typename ...T>
				Node bin_op(T ...ops) {
					return bin_op({ops...});
				}

			Node bin_op(std::initializer_list<Token::Type> ops);

	};
}// namespace



#endif
