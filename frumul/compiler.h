#ifndef COMPILER_H 
#define	COMPILER_H 
/* Contains a class which does the semantic check
 * and the compilation
 * of the symbol values
 */

#include <experimental/any>
#include <initializer_list>
#include <memory>
#include <utility>
#include "bytecode.h"
#include "functions.inl"
#include "node.h"
#include "parameters.h"
#include "retchecker.h"
#include "symbol.h"
#include "symboltab.h"
#include "util.h"
#include "value.h"
#include "vmtypes.h"

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
			__compiler (const Node& n,const ExprType& rt, Symbol& s,const bst::str& nlang);
			ByteCode compile();
		protected:
			// attributes
			const Node& node;
			ExprType return_type{ET::TEXT};
			ByteCode bytecode;
			RetChecker rtc;
			Symbol& parent;
			const bst::str& lang;
			int unsafe_args_remainder;
			std::vector<E::any>& constants{bytecode.getConstants()};
			std::vector<byte>& code{bytecode.getCode()};
			std::unique_ptr<SymbolTab> symbol_table{std::make_unique<SymbolTab>()};
			int _const_vector_pos{-1};

			// functions
			template <typename ...T>
				void appendInstructions(T... instructions) {
					appendInstructions({static_cast<byte>(instructions)...});
				}
			template <typename ...T>
				size_t insertInstructions(int i, T... instructions) {
					return insertInstructions(i,{static_cast<byte>(instructions)...});
				}
			void appendInstructions(std::initializer_list<byte> instructions);
			void appendInstructions(const ExprType&);
			size_t insertInstructions(int i, std::initializer_list<byte> instructions);

			void appendPushConstant(int i);
			void appendAndPushConstAnyVector();
			template <typename T>
				void appendAndPushConstant(const T& new_const) {
					appendPushConstant(
							bytecode.addConstant<T>(new_const)
							);
				}

			void setJump(unsigned long source, unsigned long target);

			void setReturnValue();
			VarSymbol& getOrCreateVarSymbol(const Node& name, const ExprType& type);
			ExprType visit(const Node& n);
			ExprType visit_basic_value(const Node& n,bool add_return=true);
			ExprType visit_bin_op(const Node& n);
			void visit_compare_op(const Node& n);
			ExprType visit_comparison(const Node& n);
			ExprType visit_condition(const Node& n);
			ExprType visit_index(const Node& n);
			ExprType visit_index_assignment(const Node& n);
			ExprType visit_list(const Node& n);
			ExprType visit_list_with_index(const Node& n);
			ExprType visit_list_type_declaration(const Node& n,const ExprType& primitive);
			ExprType visit_litbool(const Node& n);
			ExprType visit_litint(const Node& n);
			ExprType visit_litsym(const Node& n);
			ExprType visit_littext(const Node& n);
			ExprType visit_loop(const Node& n);
			ExprType visit_symcall(const Node& n);
			ExprType visit_unary_op(const Node& n);
			ExprType visit_unsafe_arg(const Node& n);
			ExprType visit_val_text(const Node& n);
			ExprType visit_variable_assignment(const Node& n);
			ExprType visit_variable_declaration(const Node& n);
			ExprType visit_variable_name(const Node& n);
			void cast(const ExprType& source, const ExprType& target, const Node& source_node, const Node& target_node);
			void checkVariable(const bst::str& name,const Node& n);

			void throwInconsistentType(const ExprType& t1, const ExprType& t2,const Node& n1, const Node& n2);
			void throwInconsistentType(const ExprType& t1, const ExprType& t2, const Position& n1, const Position& n2);

			void visitParameters();

	};

	class ValueCompiler : public __compiler {
		/* Compiles one value
		 */
		public:
			ValueCompiler(OneValue& v,const bst::str& nlang):
				__compiler(v.getValue(),v.getParent().getReturnType(),v.getParent(),nlang)
		{
			assert(v.getValue().type() == Node::BASIC_VALUE&&"Node should be a basic value");
		}
	};

	class MonoExprCompiler : public __compiler {
		/* Compiles an only expression
		 */
		public:
			MonoExprCompiler(const Node& n, const ExprType& et, Symbol& p, const bst::str& nlang):
				__compiler(n,et,p,nlang)
			{}

			ByteCode compile();
	};

#pragma message "verify type cast if constant"
}
#endif
