/* This file contains the Hinterpreter
 * class, which has the task to interpret
 * the header part
 */
#include "node.h"
#include "symbol.h"

namespace frumul {
	class Hinterpreter {
		/* Interprets the header
		 * and creates the Symbols
		 * tree
		 */
		public:
			Hinterpreter (Node& nheader);
			const Symbol& getSymbolTree();
		private:
			//attributes
			bool jobDone{false};
			Symbol main_symbol{};
			Node& header;

			// functions
			void visit(Node& node, Symbol& parent);
			void visit_declaration(Node& node, Symbol& parent);
			void visit_options(Node& node, Symbol& parent);
			void visit_basic_value(Node& node, Symbol& parent);
	};
}
