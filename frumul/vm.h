#ifndef VM_H
#define VM_H

/* This file contains the virtual machine
 * to execute the values of the symbols
 */

#include <cassert>
#include <experimental/any>
#include "bstrlib/bstrwrap.h"
#include "bytecode.h"
#include "functions.inl"
#include "symbol.h"
// TODO 8to16.cpp -> transform an int in char and reverse
//

namespace frumul {
	class VM {
		/* Virtual machine
		 * which executes
		 * the values
		 * of the symbols
		 * Absolutely no check is done.
		 * Bytecode must be correctly set
		 * and checked before
		 */
		public:
			VM(ByteCode&);
			template <typename T>
				T run() {
					/* Run the VM and return T
					 */
					main_loop();
					return E::any_cast<T>(variables[0]);
				}
		private:
			// functions
			void main_loop();
			void jump();
			void jump_true();
			void jump_false();
			void call();
			void cast();
			void list_append();
			void list_get_elt();
			void text_get_char();
			void push();
			void assign();
			template <typename T>
				T pop() {
					/* pops the last elements of the stack
					 */
					return E::any_cast<T>(stack.pop());
				}
			// attributes
			ByteCode& bt;
			std::vector<byte>::iterator it; // each method is responsible to modify the iterator for its own needs and must stay at the last position she used.
			// stacks. The first element of the stack matching with the return type is the return value
			rstack<E::any> stack;
			std::vector<E::any> variables; // index 0 represents the return value
	};
}
#endif