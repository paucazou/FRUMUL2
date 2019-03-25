#ifndef VM_H
#define VM_H

/* This file contains the virtual machine
 * to execute the values of the symbols
 */

#include <cassert>
#include <any>
#include <tuple>
#include "fstring.h"
#include "bytecode.h"
#include "exception.h"
#include "functions.inl"
#include "symbol.h"
//#include "header.h"
// TODO 8to16.cpp -> transform an int in char and reverse
//

namespace frumul {
	class Symbol;
	class VM {
		/* Virtual machine
		 * which executes
		 * the values
		 * of the symbols
		 * No check is done, except the following:
		 * 	- division by zero
		 * 	- index error
		 * 	- cast error
		 * Each of these errors throws an runtime error.
		 * Bytecode must be correctly set
		 * and checked before
		 */
		public:
			VM(ByteCode&,const FString& nlang,const std::vector<std::any>& args);
			template <typename T> // probably useless
				T run() {
					/* Run the VM and return T
					 */
					main_loop();
					return std::any_cast<T>(variables[0]);
				}
			std::any run();
		private:
			// functions
			void main_loop();
			void length();
			void jump();
			void jump_true();
			void jump_false();
			void call();
			void cast();
			void modulo();
			void list_append();
			void list_get_elt();
			void list_set_elt();
			void text_get_char();
			void text_set_char();
			void push();
			void assign();
			ExprType getRealType();
			template <typename T>
				T pop() {
					/* pops the last elements of the stack
					 */
#if DEBUG && 0
					printl("Position: ");
					printl(std::distance(bt.getBegin(),it));
					printl("Top of stack:");
					printl(stack.top().type().name());
#endif

					return std::any_cast<T>(stack.pop());
				}
			// attributes
			ByteCode& bt;
			std::vector<byte>::iterator it; // each method is responsible to modify the iterator for its own needs and must stay at the last position she used.
			// stacks. The first element of the stack matching with the return type is the return value
			rstack<std::any> stack;
			std::vector<std::any> variables; // index 0 represents the return value
			const FString& lang;
	};
	//using Arg = std::tuple<ExprType,FString,std::any>; // <type,name(if necessary),value>
	struct Arg {
		/* This struct is used to share
		 * the arguments of a value
		 * inside values
		 */
		ExprType type;
		FString name;
		std::any value;
		Position pos;
	};
}
#endif
