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
					printl(e);
					assert(false&&"Type unknown");
		};
		return "nothing"; // to silent clang
	}

	void ByteCode::addVariable(int i) {
		/* Increment the number of variables to allow
		 */
		v_nb += i;
	}

	ByteCode::operator bool () const {
		/* false if bytecode has
		 * not been yet set
		 */
		return !code.empty();
	}

}
