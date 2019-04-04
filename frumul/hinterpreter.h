#ifndef HINTEPRETER_H
#define HINTEPRETER_H
/* This file contains the Hinterpreter
 * (Header Interpreter)
 * class, which has the task to interpret
 * the header part
 */
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include "fdeclaration.h"
#include "functions.inl"
#include "macros.h"
#include "node.h"
//#include "transpiler.h"
#include "symbol.h"
//#include "value.h"

//#include "header.h"

namespace frumul {

	using RSymbol = std::reference_wrapper<Symbol>;
	class Fdeclaration;
	class Lang;
	class OneValue;
	class Value;

	class InheritedOptions {
		/* This simple class
		 * handles the options set
		 * by a namespace parent
		 * to be used by the children
		 */
		public:
			InheritedOptions();
			// setters
			void setLangs(const std::vector<Lang>&);
			void setMark(const Node&);
			void setParameters(const Parameters&);
			// getters
			const std::vector<Lang>& getLangs() const;
			const Mark& getMark() const;
			const Parameters& getParameters() const;
			// booleans
			bool hasMark() const;
			bool hasLangs() const;
			bool hasParameters() const;
			//display
			FString toString()const;
			STDOUT(InheritedOptions)

		private:
			std::vector<Lang> langs;
			Mark mark;
			Parameters parameters;
	};

	class Hinterpreter {
		/* Interprets the header
		 * and creates the Symbols
		 * tree
		 */
		public:
			Hinterpreter (const Node& nheader,const std::map<FString,std::unique_ptr<Symbol>>&);
			std::shared_ptr<Symbol> getSymbolTree();
		private:
			//attributes
			std::shared_ptr<Symbol> main_symbol;
			const Node& header;
			rstack<InheritedOptions> inherited_stack;
			std::stack<RSymbol> aliases;
			const std::map<FString,std::unique_ptr<Symbol>>& binary_files;

			// functions
			void visit(const Node& node, Symbol& parent);
			void visit_declaration(const Node& node, Symbol& parent);
			void visit_declaration(const Node& node, Symbol& parent,Fdeclaration&);
			Fdeclaration visit_namespace_value(const Node& val,Symbol& parent);
			OneValue& visit_options_basic(const Node& node, Symbol& sym);
			OneValue& inherit(const Node& node,Symbol& sym,std::vector<Lang>& langs);
			void visit_options_namespace (const Node&);
			void visit_basic_value(const Node& node, OneValue& oval);

			void interpret_aliases();
	};
}
#endif
