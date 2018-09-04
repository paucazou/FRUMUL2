#ifndef COMPILER_H 
#define	COMPILER_H 
/* Contains a class which does the semantic check
 * and the compilation
 * of the symbol values
 */

#include <initializer_list>
#include "bytecode.h"
#include "node.h"
#include "symbol.h"

namespace frumul {
	class Compiler {
		/* Checks that the value
		 * of the symbol is correct
		 */
		public:
			Compilation (Symbol& s, const bst::str& lang);
		private:
			Symbol& symbol;
			OneValue& val;

	};

	class __compiler {
		/* Class that compiles effectively
		 */
		public:
			__compiler (const Node& n,BT::ExprType rt, const Symbol& s);
			ByteCode compile();
		private:
			// attributes
			const Node& node;
			BT::ExprType return_type;
			ByteCode byte_code;
			std::vector<std::any>& constants{bytecode.getConstants()};
			std::vector<byte>& code{bytecode.getCode()};

			// functions
			template <typename ...T>
				void appendInstructions(T... instructions) {
					appendInstructions({static_cast<byte>(instructions)...});
				}
			void appendInstructions(std::initializer_list<byte>& instructions);
			BT::ExprType visit(const Node& n);
			BT::ExprType visit_basic_value(const Node& n);
			BT::ExprType visit_bin_op(const Node& n);
			BT::ExprType visit_litint(const Node& n);

			void throwInconsistentType(BT::ExprType t1, BT::ExprType t2,const Node& n1, const Node& n2);

	}
#error "verify type"
#error "verify type cast if constant"
#error "verify call to functions that exist"
#error "verify that variables have been declared"
#error "verify that variables have been assigned before used"
}
#endif
