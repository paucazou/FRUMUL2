#ifndef COMPILER_H 
#define	COMPILER_H 
/* Contains a class which does the semantic check
 * and the compilation
 * of the symbol values
 */

#include <experimental/any>
#include <initializer_list>
#include <memory>
#include "bytecode.h"
#include "node.h"
#include "symbol.h"
#include "symboltab.h"
#include "value.h"
//#include "header.h"

namespace E = std::experimental;

namespace frumul {

	class OneValue;
	class SymbolTab;
	class Value;
	class VarSymbol;

	class __compiler {
		/* Class that compiles effectively
		 */
		public:
			__compiler (const Node& n,BT::ExprType rt, Symbol& s);
			ByteCode compile();
		private:
			// attributes
			const Node& node;
			BT::ExprType return_type{BT::TEXT};
			ByteCode bytecode;
			std::vector<E::any>& constants{bytecode.getConstants()};
			std::vector<byte>& code{bytecode.getCode()};
			std::unique_ptr<SymbolTab> symbol_table{std::make_unique<SymbolTab>()};

			// functions
			template <typename ...T>
				void appendInstructions(T... instructions) {
					appendInstructions({static_cast<byte>(instructions)...});
				}
			template <typename ...T>
				void insertInstructions(int i, T... instructions) {
					insertInstructions(i,{static_cast<byte>(instructions)...});
				}
			void appendInstructions(std::initializer_list<byte> instructions);
			void insertInstructions(int i, std::initializer_list<byte> instructions);

			void appendPushLastConstant();

			void setReturnValue();
			BT::ExprType visit(const Node& n);
			BT::ExprType visit_basic_value(const Node& n);
			BT::ExprType visit_bin_op(const Node& n);
			void visit_compare_op(const Node& n);
			BT::ExprType visit_comparison(const Node& n);
			BT::ExprType visit_litbool(const Node& n);
			BT::ExprType visit_litint(const Node& n);
			BT::ExprType visit_unary_op(const Node& n);
			BT::ExprType visit_val_text(const Node& n);
			BT::ExprType visit_variable_assignment(const Node& n);
			BT::ExprType visit_variable_declaration(const Node& n);
			BT::ExprType visit_variable_name(const Node& n);

			void throwInconsistentType(BT::ExprType t1, BT::ExprType t2,const Node& n1, const Node& n2);
			void throwInconsistentType(BT::ExprType t1, BT::ExprType t2, const Position& n1, const Position& n2);

	};

	class ValueCompiler : public __compiler {
		/* Compiles one value
		 */
		public:
			ValueCompiler(OneValue& v):
				__compiler(v.getValue(),v.getParent().getReturnType(),v.getParent())
		{
			assert(v.getValue().type() == Node::BASIC_VALUE&&"Node should be a basic value");
		}
	};
#pragma message "verify type cast if constant"
#pragma message "verify call to functions that exist"
}
#endif
