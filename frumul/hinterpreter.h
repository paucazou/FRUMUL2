#ifndef HINTEPRETER_H
#define HINTEPRETER_H
/* This file contains the Hinterpreter
 * (Header Interpreter)
 * class, which has the task to interpret
 * the header part
 */
#include <cassert>
#include <stack>
#include "fdeclaration.h"
#include "node.h"
#include "symbol.h"

namespace frumul {
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
			// getters
			const std::vector<Lang>& getLangs() const;
			const Mark& getMark() const;
		private:
			std::vector<Lang> langs;
			Mark mark;
			//parameters
	};

	class Hinterpreter {
		/* Interprets the header
		 * and creates the Symbols
		 * tree
		 */
		public:
			Hinterpreter (const Node& nheader);
			const Symbol& getSymbolTree();
		private:
			//attributes
			bool jobDone{false};
			Symbol main_symbol{};
			const Node& header;
			std::stack<InheritedOptions> inherited_stack;

			// functions
			void visit(const Node& node, Symbol& parent);
			void visit_declaration(const Node& node, Symbol& parent);
			void visit_declaration(const Node& node, Symbol& parent,Fdeclaration&);
			Fdeclaration visit_namespace_value(const Node& val,Symbol& parent);
			OneValue& visit_options_basic(const Node& node, Symbol& sym);
			void visit_options_namespace (const Node&);
			void visit_basic_value(const Node& node, OneValue& oval);
	};
}
#endif
