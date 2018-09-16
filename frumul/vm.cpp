#include "vm.h"

// macros
constexpr int address_size = 2; // should be used everywhere an address is required
#define ASSERT_ADDRESS_SIZE static_assert(address_size == 2,"Address size is supposed to be 2")

#define BINARY_OP(op,T,U) \
		do { \
			auto a {pop<T>()};\
			auto b {pop<U>()};\
			printl("a = " << a);\
			printl("b = " << b);\
			stack.push(a op b);\
		} while(false) // to allow the semi colon

#define UNARY_OP(op,T) \
	stack.push(op pop<T>())

#define BINARY_OP_SAME_TYPE(op,T) BINARY_OP(op,T,T)
#define BINARY_OP_INT(op) BINARY_OP_SAME_TYPE(op,int)
#define BINARY_OP_BOOL(op) BINARY_OP_SAME_TYPE(op,bool)

#define LIST_PUSH(T) E::any_cast<std::vector<T>>(list).push_back(pop<T>())

/* COMPARE syntax
 * 	COMPARISON_TYPE (BOOL_EQUAL,BOOL_INFERIOR,etc.)
 * 	TYPE (TEXT,INT, BOOL, etc.)
 */
#define COMPARE(op) \
	do { \
	BT::ExprType t{static_cast<BT::ExprType>(*++it)}; \
	switch (t) { \
		case BT::TEXT: \
			       stack.push(pop<bst::str>() op pop<bst::str>()); \
				break; \
		case BT::INT: \
			      stack.push(pop<int>() op pop<int>()); \
				break; \
		case BT::BOOL: \
			       stack.push(pop<bool>() op pop<bool>()); \
				break; \
		default: \
			 assert(false&&"Type unknown"); \
	}; \
	} while (false) // to allow the semi colon
	
namespace frumul {
	VM::VM(ByteCode& nbt,const bst::str& nlang) :
		bt{nbt}, it{nbt.getBegin()}, lang{nlang}
	{
		// resize variables vector
		variables.resize(bt.getVariableNumber());
		// set the return value to an empty string if it is a TEXT
		if (bt.getReturnType() == BT::TEXT)
			variables[0] = bst::str{""};
		// set an arbitrary number of elements for the stack 
		// but it can be set above without problem
		stack.exposeContainer().resize(255);

	}

	E::any VM::run() {
		/* run vm and return value
		 */
		main_loop();
		return variables[0];
	}

	void VM::main_loop() {
		/* Main loop.
		 * Follow the instructions until the end.
		 * This function only manages the instructions
		 */
		while (it != bt.getEnd()) {
			BT::Instruction in {static_cast<BT::Instruction>(*it)};
			switch (in) {
				// preponed arguments
				// Syntax is limited to the instruction
				case BT::INT_ADD:	BINARY_OP_INT(+); break;
				case BT::INT_MUL:	BINARY_OP_INT(*); break;
				case BT::INT_DIV:	BINARY_OP_INT(/); break;
				case BT::INT_SUB:	BINARY_OP_INT(-); break;
				case BT::INT_MOD:	BINARY_OP_INT(%); break;
				case BT::INT_NEG:	UNARY_OP(-,int); break;
				case BT::INT_POS:	UNARY_OP(+,int); break;
				case BT::TEXT_ADD:	BINARY_OP_SAME_TYPE(+,bst::str);break;
				case BT::TEXT_MUL:	BINARY_OP(*,bst::str,int);break;
				case BT::BOOL_AND:	BINARY_OP_BOOL(&&);break;
				case BT::BOOL_OR:	BINARY_OP_BOOL(||);break;
				case BT::BOOL_NOT:	UNARY_OP(!,bool); break;

				// postponed arguments
				case BT::BOOL_EQUAL:	COMPARE(==);break;
				case BT::BOOL_INFERIOR:	COMPARE(<);break;
				case BT::BOOL_SUPERIOR:	COMPARE(>);break;
				case BT::BOOL_INF_EQUAL:COMPARE(<=);break;
				case BT::BOOL_SUP_EQUAL:COMPARE(>=);break;
				case BT::TEXT_GET_CHAR:	
				case BT::LIST_APPEND:	list_append();break;
				case BT::LIST_GET_ELT: 	list_get_elt();break;
				case BT::JUMP_TRUE: 	jump_true();break;
				case BT::JUMP_FALSE:	jump_false();break;
				case BT::JUMP:		jump(); break;
				case BT::CALL:		call();break;
				case BT::CAST:		cast();break;
				case BT::ASSIGN:	assign();break;
				case BT::PUSH: 		push(); break;

				case BT::RETURN:	return;break;

				default:
					assert(false&&"Instruction not recognized");
			};
			++it; // increment the iterator to manage the next instruction
		}
	}

	void VM::jump_true() {
		/* Jump to the required place if last bool
		 * is true. Else go just after the address.
		 * See jump() for more info.
		 * Syntax:
		 * 	JUMP_TRUE
		 * 	ADDRESS_1
		 * 	ADDRESS_2
		 */
		ASSERT_ADDRESS_SIZE;
		if (pop<bool>())
			jump();
		else
			it += 2; // skip the address and go on
	}

	void VM::jump_false() {
		/* Jump if last bool is false
		 * inverse of conditional_jump
		 * Syntax:
		 * 	JUMP_FALSE
		 * 	ADDRESS_1
		 * 	ADDRESS_2
		 */
		ASSERT_ADDRESS_SIZE;
		if (pop<bool>())
			it += 2;
		else
			jump();
	}

	void VM::jump() {
		/* Unconditional jump: go to the 
		 * required relative place and exits
		 * Syntax:
		 * 	JUMP
		 * 	ADDRESS_1
		 * 	ADDRESS_2
		 */
		// NOTE: the number of elements distant from the jump in the code is limited to sizeof(short)*256
		// Two bytes are required to make an adress: the first byte represents the end and the second the start. Although each byte is an unsigned one, the short that they form may be negative or positive since they must be created by the split of a short.

		ASSERT_ADDRESS_SIZE;
		uint_fast16_t address{*++it};
		address = address << 8; // push the first byte to its place: the greater importance
		address += *++it; // set the 8 first bits with the second byte
		// set the iterator. It should be on an instruction
		it += address;
	}

	void VM::call() {
		/* Call another value
		 * Syntax:
		 * 	CALL
		 * 	ARG_NUMBER
		 */
		// create the list to return
		std::vector<E::any> args;

		// iterate to get the arguments
		for (int arg_byte_nb{*++it};arg_byte_nb > 0;++arg_byte_nb)
			args.push_back(stack.pop());

		// get the symbol to call
		Symbol& s{pop<RSymbol>().get()};
		
		// call and push if not void
		if (s.getReturnType() != BT::VOID)
			stack.push(s.any_call(args));
	}
	
	void VM::cast() {
		/* Cast a value of one type to another type
		 * if possible
		 * syntax:
		 * 	CAST
		 * 	SOURCE_TYPE
		 * 	TARGET_TYPE
		 */
#pragma message("Do not forget to catch errors at runtime")
		BT::ExprType source_t {static_cast<BT::ExprType>(*++it)};
		BT::ExprType target_t {static_cast<BT::ExprType>(*++it)};
		switch (source_t) {
			case BT::TEXT:
				switch (target_t) {
					case BT::INT:
						stack.push(static_cast<int>(pop<bst::str>()));
						break;
					case BT::BOOL:
						{
							bst::str s{pop<bst::str>()};	
							if (s == "1" || s == "true")
								stack.push(true);
							else
								stack.push(false);
						
					   	}
						break;
					case BT::SYMBOL:
						{
							bst::str s{pop<bst::str>()};
							Symbol& parent{bt.getParent()};
							stack.push(parent.getChildren().find(s,PathFlag::Relative));
						}
						break;
					default:
						assert(false&&"Type unknown");
				};
				break;

			case BT::INT:
				switch (target_t) {
					case BT::TEXT:
						stack.push(bst::str(pop<int>()));
						break;
					case BT::BOOL:
						stack.push(static_cast<bool>(pop<bool>()));
						break;
					default:
						assert(false&&"Type unknown");
					};
				break;
			case BT::BOOL:
				switch(target_t) {
					case BT::TEXT:
						{
						bool b{pop<bool>()};
						const bst::str True{"true"}, False{"false"};
						if (b)
							stack.push(True);
						else
							stack.push(False);
						}
						break;
					case BT::INT:
						stack.push(static_cast<int>(pop<bool>()));
						break;
					default:
						assert(false&&"Type unkwnown");
				};
				break;
			default:
				assert(false&&"Type canno't be cast");
		};

	}

	void VM::list_append() {
		/* Append last element of stack into
		 * specific list
		 * Syntax:
		 * 	LIST_APPEND
		 * 	TYPE
		 * 	INDEX
		 */
		BT::ExprType t{static_cast<BT::ExprType>(*++it)};
		E::any list{variables[*++it]};

		switch (t) {
			case BT::INT:	LIST_PUSH(int);break;
			case BT::TEXT:	LIST_PUSH(bst::str);break;
			case BT::SYMBOL:LIST_PUSH(RSymbol);break;
			case BT::BOOL:	LIST_PUSH(bool);break;
			default:
				assert(false&&"Type unknown");
		};
	}

	void VM::text_get_char() {
		/* Get the char of a string
		 * Syntax:
		 * 	TEXT_GET_CHAR
		 * 	TYPE(CONST OR NOT)
		 * 	INDEX_STRING
		 * 	INDEX_ELT
		 */
#pragma message("catch errors at runtime")
		// get type
		BT::ExprType t{static_cast<BT::ExprType>(*++it)};
		// get string
		const E::any& s = (t & BT::CONSTANT ?
				&bt.getConstant(*++it) :
				&variables[*++it]
			    );

		// get element
		stack.push(E::any_cast<bst::str>(s).uAt(*++it));
	}

	void VM::list_get_elt() {
		/* Get an element of a list
		 * and push it into the stack
		 * Syntax:
		 * 	LIST_GET_ELT
		 * 	TYPE (CONST OR NOT)
		 * 	INDEX_LIST
		 * 	INDEX_ELT
		 */
#pragma message("We must catch errors at runtime")
		// get type
		BT::ExprType t{static_cast<BT::ExprType>(*++it)};
		// get list
		const E::any& list {(t & BT::CONSTANT ?
				bt.getConstant(*++it) :
				variables[*++it]
			       )};
		// get element
		int i{*++it};
		if (t & BT::INT)
			stack.push(E::any_cast<std::vector<int>>(list)[i]);
		else if (t & BT::TEXT)
			stack.push(E::any_cast<std::vector<bst::str>>(list)[i]);
		else if (t & BT::SYMBOL)
			stack.push(E::any_cast<std::vector<RSymbol>>(list)[i]);
		else if (t & BT::BOOL)
			stack.push(E::any_cast<std::vector<bool>>(list)[i]);
	}
		
	void VM::push() {
		/* push a value in one of the stacks
		 * number of variables and constants are limited to 256
		 * TODO maybe change this number to a short
		 * Syntax:
		 * 	PUSH
		 * 	TYPE (CONST OR VAR)
		 * 	INDEX
		 */
		BT::ExprType t{static_cast<BT::ExprType>(*++it)};
		int i{*++it};

		if (t == BT::CONSTANT) {
			stack.push(bt.getConstant(i));
		}
		else {
			stack.push(variables[i]);
		}
	}

	void VM::assign() {
		/* Assign the last elt of a stack
		 * into a variable
		 * Syntax
		 * 	ASSIGN
		 * 	INDEX
		 */
		variables[*++it] = stack.pop();
	}

}
