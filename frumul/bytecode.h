#ifndef BYTECODE_H
#define BYTECODE_H

/* This file contains the bytecode
 * produced by the compiler
 */

#include <cassert>
#include <cstdint>
#include <experimental/any>
#include <functional>
#include <map>
#include <vector>
#include "bstrlib/bstrwrap.h"
//#include "header.h"

namespace E = std::experimental;

namespace frumul {
	class Symbol;

	using byte = uint_fast8_t;
	constexpr unsigned int MAX_VARIABLES{sizeof(byte)};

	class ByteCode {
		/* Bytecode representation
		 * of the values
		 */
		public:
			enum ExprType {
				/* Type of the variables, constants
				 * and other expressions
				 */
				VOID		= 0 << 0,
				INT 		= 1 << 0,
				TEXT 		= 1 << 1,
				SYMBOL 		= 1 << 2,
				BOOL 		= 1 << 3,

				CONSTANT 	= 1 << 4,
				VARIABLE	= 1 << 5,
				MAX_EXPR_TYPE,
			};

			enum Instruction {
				/* Instruction type
				 */
				NULL_INSTRUCTION, // equal to zero
				// Arguments preponed: every arg is before the instruction
				// and must be inside a stack 
				// int
				INT_ADD,
				INT_DIV,
				INT_SUB,
				INT_NEG,
				INT_MUL,
				// // bools
				BOOL_AND,
				BOOL_OR,
				BOOL_NOT,
				// // texts
				TEXT_ADD,
				TEXT_MUL,
				TEXT_GET_CHAR,
				// Arguments postponed: at least one arg is after the instruction, and can use the last item of a stack
				BOOL_EQUAL,
				BOOL_INFERIOR,
				BOOL_SUPERIOR,
				BOOL_INF_EQUAL,
				BOOL_SUP_EQUAL,

				LIST_APPEND,
				LIST_GET_ELT,
				JUMP_TRUE, // jump with a bool before and the emplacement to go after, ignored if bool is false
				JUMP_FALSE, // inverse of CONDITIONAL_JUMP
				JUMP, // jump with the emplacement to go just after, relative to the last part of the emplacement
				CALL, // call another symbol 
				CAST, // cast one value of a type to another, if possible
				ASSIGN,
				PUSH,
				RETURN, 

				MAX_INSTRUCTIONS,
			};
			explicit ByteCode(Symbol& s);
			std::vector<byte>::iterator getBegin();
			std::vector<byte>::iterator getEnd();
			int getVariableNumber() const;
			ExprType getReturnType() const;
			Symbol& getParent() ;
			
			const E::any& getConstant(size_t) const;
			std::vector<E::any>& getConstants();
			std::vector<byte>& getCode();
			static bst::str typeToString(ExprType);

			void addVariable(int i=1);

			operator bool () const;
		private:
			ExprType return_type{TEXT};
			Symbol& parent;
			std::vector<byte> code;
			// constants
			std::vector<E::any> constants;
			// number of variables
			int v_nb{0};
	};
	using BT = ByteCode;
}
#endif
