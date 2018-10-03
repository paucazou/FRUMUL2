#include "bytecode.h"
#include "macros.h"

namespace frumul {

	ByteCode::ByteCode(Symbol& p) :
		parent{p}
	{
	}

	std::vector<byte>::iterator ByteCode::getBegin() {
		/* Get the iterator pointing to the first
		 * element of the code
		 */
		return code.begin();
	}

	std::vector<byte>::iterator ByteCode::getEnd() {
		/* Get the iterator pointing
		 * to the end of the code
		 */
		return code.end();
	}

	int ByteCode::getVariableNumber() const {
		/* Get the number of variables
		 * that will be used at runtime
		 */
		return v_nb;
	}

	ByteCode::ExprType ByteCode::getReturnType() const {
		/* Return return type
		 */
		return return_type;
	}

	Symbol& ByteCode::getParent() {
		/* Get the parent of the value
		 */
		return parent;
	}

	const E::any& ByteCode::getConstant(size_t i) const {
		/* Return the value matching
		 * with i in constants
		 */
		assert(i < constants.size() && "Constants index is too large");
		return constants[i];
	}

	std::vector<E::any>& ByteCode::getConstants() {
		return constants;
	}

	std::vector<byte>& ByteCode::getCode() {
		return code;
	}

	bst::str ByteCode::typeToString(BT::ExprType e) {
		/* Return a string representation of type
		 */
		switch (e) {
			case INT:	return "INT";
			case TEXT:	return "TEXT";
			case BOOL:	return "BOOL";
			case SYMBOL:	return "SYMBOL";
			case VOID:	return "VOID";
			default:
					if (e >= LIST)
						return listToString(e);
						
					else {
						printl(e);
						assert(false&&"Type unknown");
					}
		};
		return "nothing"; // to silent clang
	}

	bst::str ByteCode::listToString(ExprType e) {
		/* String representation of a list
		 */
		bst::str s{"LIST "};
		// depth
		int depth{0};
		while (e >= LIST) {
			e = static_cast<ExprType>(e - LIST);
			depth += 1;
		}
		s += bst::str(depth) + " ";
		// type
		s += typeToString(e);
		return s;
	}

	void ByteCode::addVariable(int i) {
		/* Increment the number of variables to allow
		 */
		v_nb += i;
	}

	void ByteCode::addRuntimeError(const BaseException& e) {
		/* Add an exception that can be raised
		 * by the vm on runtime if necessary.
		 * It keeps the error in a map called runtime_errors
		 * with the position of the last instruction added
		 * as the key.
		 * So call this function after the last instruction
		 * you want the exception to be raised.
		 */
		runtime_errors.insert({
				code.size() - 1,e
				});
	}

	void ByteCode::throwRuntimeError(unsigned int i) {
		/* Throw an error during the runtime
		 * The error must be set by the compiler
		 */
		throw runtime_errors.at(i);
	}

	ByteCode::operator bool () const {
		/* false if bytecode has
		 * not been yet set
		 */
		return !code.empty();
	}

	// other functions

	AnyVector& operator + (AnyVector& a, const AnyVector& b) {
		/* Add the content of b in a and return
		 * a reference to a
		 */
		a.insert(a.end(), // where to add b
			b.begin(),b.end()); // content of b
		return a;
	}


}
