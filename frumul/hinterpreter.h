#ifndef HINTEPRETER_H
#define HINTEPRETER_H
/* This file contains the Hinterpreter
 * (Header Interpreter)
 * class, which has the task to interpret
 * the header part
 */
#include <memory>
#include "fdeclaration.h"
#include "node.h"
#include "symbol.h"

namespace frumul {
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

			// functions
			void visit(const Node& node, Symbol& parent);
			void visit_declaration(const Node& node, Symbol& parent);
			void visit_declaration(const Node& node, Symbol& parent,Fdeclaration&);
			Fdeclaration visit_namespace_value(const Node& val,Symbol& parent);
			OneValue& visit_options(const Node& node, Symbol& parent);
			void visit_basic_value(const Node& node, OneValue& oval);
	};
}
#endif
