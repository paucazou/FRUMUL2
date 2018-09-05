#include "compiler.h"
// Note: do not forget that what is at right should be push in the stack first

namespace frumul {

	Compiler::Compiler(Symbol& s, const bst::str& lang) :
		symbol{s}, val{s.getValue().getValue(lang)};
#pragma message("Parameters are not yet used")
	{
		// check if lang exists for this value
		if (!s.getValue().hasLang(lang))
			throw exc(exc::LangNotSet,lang + ": has not been set for this symbol", s.toString());

		// compilation of the parameters, if necessary
		// compilation of the value
		__compiler c{val.getValue(),s.getReturnType()};
		val.setByteCode(c.compile());

	}

	// __compiler

	__compiler::__compiler (const Node& n, BT::ExprType rt, const Symbol& s) :
		node{n}, return_type{rt}, bytecode{s}
	{
		assert(n.type() == Node::BASIC_VALUE&&"Node should be a basic value");
	}

	ByteCode __compiler::compile() {
		/* Compile the node into a bytecode
		 */
		if (!bytecode)
			visit(node);
		return bytecode;
	}

	void __compiler::appendInstructions(std::initializer_list<byte>& instructions) {
		/* Append instructions to code
		 */
		for (auto& i : instructions)
			code.push_back(i);
	}

	BT::ExprType __compiler::visit(const Node& n) {
		/* Dispatch the node following
		 * its type
		 */
		switch(n.type()) {
			case Node::BASIC_VALUE:		return visit_basic_value(n);
			case Node::BIN_OP:		return visit_bin_op(n);
			case Node::LITINT:		return visit_litint(n);
			default:
				assert(false&&"Node not recognized");
		};
		return BT::VOID; // because clang complains
	}

	BT::ExprType __compiler::visit_basic_value(const Node& n) {
		/* Visit basic value
		 */
		for (const auto& child : n.getNumberedChildren()) {
			if (rt == BT::TEXT)
				// prepare the stack
				appendInstructions(BT::PUSH,BT::VARIABLE,0);

			BT::ExprType rt{visit(child)};

			// cast if necessary
			if (rt != return_type) {
				if (return_type != BT::TEXT)
					throwInconsistentType(return_type,rt,n,child);
				else
					// cast
					appendInstructions(BT::CAST,rt,return_type);
			}
			// set returned value or append it
			if (rt != BT::TEXT)
				appendInstructions(BT::ASSIGN,0,BT::RETURN); // assign last elt of stack to return value and return
			else
				appendInstructions(BT::TEXT_ADD, // add returned to return value
						BT::ASSIGN,0); // assign back to returned value

		}
		code.push_back(BT::RETURN);
		return return_type;
	}

	BT::ExprType __compiler::visit_bin_op(const Node& n) {
		/* Compile an expression x op x
		 * and return the type
		 */
		BT::ExprType left{visit(n.get("right"))};
		switch (left) {
			case BT::INT:
				{
					BT::ExprType t2{visit(n.get("left"))};
					if (t2 != BT::INT)
						throwInconsistentType(BT::INT,t2,n.get("left"),n.get("right"));

					constexpr std::map<bst::str,BT::Instruction> types{ {"+",BT::INT_ADD},{"-",BT::INT_SUB}, {"*",BT::INT_MUL},{"/",BT::INT_DIV}};
					code.push_back(types[n.getValue()]);
					return BT::INT;
				}
			case BT::TEXT:
				{
					BT::ExprType t2{visit(n.get("left"))};
					if (n.getValue() == "+") {
						if (t2 != BT::TEXT)
							throwInconsistentType(BT::TEXT,t2,n.get("left"),n.get("right"));
						code.push_back(BT::TEXT_ADD);
					}
					else if (n.getValue() == "*") {
						if (t2 != BT::INT)
							throwInconsistentType(BT::TEXT,t2,n.get("left"),n.get("right"));
						code.push_back(BT::TEXT_MUL);
					}
					else
						throw exc(exc::InvalidOperator,n.getValue() + " can not be used with type TEXT", n.getPosition());
					return BT::TEXT;
				}
			default:
				throw exc(exc::InvalidType,n.getValue() + " can not be used with type " + BT::typeToString(left));

		};
		return BT::VOID;

	}

	BT::ExprType __compiler::visit_litint(const Node& n) {
		/* Compile a litteral integer
		 */
		constants.push_back(static_cast<int>(n.getValue()));
		appendInstructions(BT::PUSH,BT::CONSTANT,constants.size()-1);
	}

	void throwInconsistentType(BT::ExprType t1, BT::ExprType t2, const Node& n1, const Node& n2) {
	/* Throw an inconsistent error
	 */
	bst::str msg1 {BT::typeToString(t1) + " can not be used with " + BT::typeToString(t2)};
	bst::str msg2 {BT::typeToString(t2) + " defined here: "};
	throw iexc(exc::InconsistantType,msg1,n1.getPosition(), msg2, n2.getPosition());
	}
}
