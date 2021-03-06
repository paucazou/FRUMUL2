#ifndef BYTECODE_H
#define BYTECODE_H

/* This file contains the bytecode
 * produced by the compiler
 */

#include <cassert>
#include <cstdint>
#include "valvar.h"
#include <functional>
#include <map>
#include <utility>
#include <vector>
#include "fstring.h"
#include "exception.h"
#include "functions.inl"
#include "macros.h"
#include "vmtypes.h"




namespace frumul {
	class Symbol;

	using byte = uint_fast8_t;
	constexpr unsigned int MAX_VARIABLES{sizeof(byte)};

	class ByteCode {
		/* Bytecode representation
		 * of the values
		 */
		public:
			/* DEPRECATED
			enum ExprType : byte {
				* Type of the variables, constants
				 * and other expressions
				 *
				VOID		= 0 << 0,
				INT 		= 1 << 0,
				TEXT 		= 1 << 1,
				SYMBOL 		= 1 << 2,
				BOOL 		= 1 << 3,

				LIST		= 1 << 4,

				CONSTANT 	= 1 << 4, // equal to LIST, but not used in same context
				VARIABLE	= 1 << 5,
				STACK_ELT	= 1 << 6,
				MAX_EXPR_TYPE,
			};
			*/


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
				INT_MOD,
				INT_POS,
				INT_MUL,
				// // bools
				BOOL_AND,
				BOOL_OR,
				BOOL_NOT,
				// // texts
				TEXT_ADD,
				TEXT_MUL,
				TEXT_GET_CHAR,
				TEXT_SET_CHAR,
				// Arguments postponed: at least one arg is after the instruction, and can use the last item of a stack
				BOOL_EQUAL,
				BOOL_INFERIOR,
				BOOL_SUPERIOR,
				BOOL_INF_EQUAL,
				BOOL_SUP_EQUAL,

                                FIND_SYMBOL,
                                CHECK_TYPE,

				LIST_ADD,
				LIST_APPEND,
				LIST_GET_ELT,
				LIST_SET_ELT,

				LENGTH,

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
			//explicit ByteCode(const ByteCode& other);
			std::vector<byte>::iterator getBegin();
			std::vector<byte>::iterator getEnd();
			int getVariableNumber() const;
			const ExprType& getReturnType() const;
			Symbol& getParent() ;
			
			const ValVar& getConstant(size_t) const;
			std::vector<ValVar>& getConstants();
			std::vector<byte>& getCode();
			const std::map<unsigned int,ValVar>& getStatics() const;
			const Position& getEltPosition(unsigned int i) const;
			void setEltPosition(const Position& npos);

			void addVariable(int i=1);
			void addStaticVar(unsigned int);
			void addStaticVar(unsigned int,const ValVar&);
			void setStatics(const std::vector<ValVar>&);
			void addRuntimeError(const BaseException&);
			void throwRuntimeError(unsigned int i, exc::Type t);

			operator bool () const;

			template <typename T>
				int addConstant(const T& new_const) {
					/* Add a new constant.
					 * Return the index of the constant.
					 * If the constant already exists,
					 * return the index
					 */
					// try to find constant
					int i{0};
					for (auto it{constants.begin()}; it != constants.end(); ++it,++i) {
						try {
							if (it->as<T>() == new_const)
								return i;
						} catch (std::bad_variant_access& bva) {
							continue;
						}
					}

					// fails to find: add it
					constants.push_back(new_const);
					return constants.size() - 1;

				}

		private:
			ExprType return_type{ET::TEXT};
			Symbol& parent;
			std::vector<byte> code;
			// constants
			std::vector<ValVar> constants;
			// number of variables
			int v_nb{0};
			// errors at runtime
			std::map<std::pair<unsigned int,BaseException::Type>,BaseException> runtime_errors;
			// store positions of elements that must be found at runtime
			std::map<unsigned int, Position> element_positions;
			// static variables
			std::map<unsigned int, ValVar> statics;
	};
	using BT = ByteCode;
	using AnyVector = std::vector<ValVar>;
	using IntExcPair = std::pair<unsigned int, BaseException::Type>;
	
	// functions related to the bytecode
	AnyVector& operator + (AnyVector&, const AnyVector&);
}
#endif
