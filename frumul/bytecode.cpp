#include "bytecode.h"
#include "macros.h"

namespace frumul {

	ByteCode::ByteCode(Symbol& p) :
		parent{p}
	{
	}

#if 0
	ByteCode::ByteCode(const ByteCode& other) :
		return_type{other.return_type}, parent{other.parent}, code{other.code}, constants{other.constants}, v_nb{other.v_nb}, runtime_errors{other.runtime_errors}
	{
	}
#endif

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

	const ExprType& ByteCode::getReturnType() const {
		/* Return return type
		 */
		return return_type;
	}

	Symbol& ByteCode::getParent() {
		/* Get the parent of the value
		 */
		return parent;
	}

	const ValVar& ByteCode::getConstant(size_t i) const {
		/* Return the value matching
		 * with i in constants
		 */
		assert(i < constants.size() && "Constants index is too large");
		return constants[i];
	}

	std::vector<ValVar>& ByteCode::getConstants() {
		return constants;
	}

	std::vector<byte>& ByteCode::getCode() {
		return code;
	}

	const std::map<unsigned int,ValVar>& ByteCode::getStatics() const {
		return statics;
	}

	/*FString ByteCode::typeToString(BT::ExprType e) {
		* Return a string representation of type
		 *
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
						assert(false&&"Type unknown");
					}
		};
		return "nothing"; // to silent clang
	}

	FString ByteCode::listToString(ExprType e) {
		/ String representation of a list
		 *
		FString s{"LIST "};
		// depth
		int depth{0};
		while (e >= LIST) {
			e = static_cast<ExprType>(e - LIST);
			depth += 1;
		}
		s += FString(depth) + " ";
		// type
		s += typeToString(e);
		return s;
	}*/

	void ByteCode::addVariable(int i) {
		/* Increment the number of variables to allow
		 */
		v_nb += i;
	}

	void ByteCode::addStaticVar(unsigned int i) {
		/* Add a static variable with an empty value
		 */
		assert(statics.count(i) == 0&&"Value already set");
		statics[i] = ValVar();
	}

	void ByteCode::addStaticVar(unsigned int i, const ValVar& val) {
		/* Add a static variable and give it a value
		 */
		assert(statics.count(i) == 0&&"Value already set");
		statics[i] = val;
	}

	void ByteCode::setStatics(const std::vector<ValVar>& variables) {
		/* Set the static variables
		 */
		for (size_t i{0}; i<variables.size(); ++i) {
			if (statics.count(i) > 0)
				statics[i] = variables[i];
		}
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
				IntExcPair(code.size() - 1,e.getType()),e
				});
	}

	void ByteCode::throwRuntimeError(unsigned int i, exc::Type t) {
		/* Throw an error during the runtime
		 * The error must be set by the compiler
		 */
		throw runtime_errors.at(IntExcPair(i,t));
	}

	const Position& ByteCode::getEltPosition(unsigned int i) const {
		/* Return the position linked to i
		 */
		return element_positions.at(i);
	}

	void ByteCode::setEltPosition(const Position& npos) {
		/* Set a position matching with the current
		 * code position
		 */
		element_positions.insert({
				code.size() - 1,
				npos
				});
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
