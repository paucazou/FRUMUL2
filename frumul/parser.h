#ifndef PARSER_H
#define PARSER_H

#include <functional>
#include <map>
#include <memory>
#include "argcollector.h"
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
			Node types ();
			Node comparison ();
			Node expr (); // DEPRECATED
			Node term (); // DEPRECATED
			Node factor ();
			Node reference ();
			Node index ();

			Node symbol_call_or_litteral();
			Node call_arguments();

			Node loop ();
			Node condition ();

			Node path_value ();
			Node file_content (const Node& path_node);

			Node namespace_value (const int start);
			Node short_name ();
			Node long_name ();

			Node options ();
			Node simple_option(Token::Type, Node::Type,std::function<Node(Parser&)> f=std::function<Node(Parser&)>());
			std::map<bst::str,Node> lang_option ();
			std::vector<Node> param_option ();
			Node param_value ();
			std::vector<Node> arg_number ();

			Node litteral ();
			Node list ();

			Node text ();
			Node tag();
			Node arg();

			Node left_bin_op(std::initializer_list<Token::Type>::iterator);

			int _end_of_arg();
			void _manage_privileged_parameter(const bst::str&, const Position&, ArgCollector&);
			void _manage_parameter_name_rest_of_tail(const bst::str&, const Position&,ArgCollector&);

			template <typename ...T>
				Node bin_op(T ...ops) {
					return bin_op({ops...});
				}

			Node bin_op(std::initializer_list<Token::Type> ops);

	};

	using ParserFunction = std::function<Node(Parser&)>;
}// namespace



#endif
