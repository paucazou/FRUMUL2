#include "compiler.h"
// Note: do not forget that what is at right should be push in the stack first

namespace frumul {

	/*
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
	*/

	// __compiler

	__compiler::__compiler (const Node& n, BT::ExprType rt, Symbol& s) :
		node{n}, return_type{rt}, bytecode{s}
	{
	}

	ByteCode __compiler::compile() {
		/* Compile the node into a bytecode
		 */
		if (!bytecode) {
			bytecode.addVariable();
			visit(node);
		}
		return bytecode;
	}

	void __compiler::appendInstructions(std::initializer_list<byte> instructions) {
		/* Append instructions to code
		 */
		for (auto& i : instructions)
			code.push_back(i);
	}



	BT::ExprType __compiler::visit(const Node& n) {
		/* Dispatch the node following
		 * its type
		 * If the function called return void, this one returned the return type expected
		 */
		switch(n.type()) {
			case Node::BASIC_VALUE:		return visit_basic_value(n);
			case Node::BIN_OP:		return visit_bin_op(n);
			case Node::COMPARISON:		return visit_comparison(n);
			case Node::LITINT:		return visit_litint(n);
			case Node::VARIABLE_DECLARATION:
							visit_variable_declaration(n);
							return return_type;
							break;
			default:
				assert(false&&"Node not recognized");
		};
		return BT::VOID; // because clang complains
	}

	BT::ExprType __compiler::visit_basic_value(const Node& n) {
		/* Visit basic value
		 */
		for (const auto& child : n.getNumberedChildren()) {
			if (return_type == BT::TEXT)
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
			if (return_type != BT::TEXT)
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

					static std::map<bst::str,BT::Instruction> types{ {"+",BT::INT_ADD},{"-",BT::INT_SUB}, {"*",BT::INT_MUL},{"/",BT::INT_DIV}};
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
				throw exc(exc::TypeError,n.getValue() + " can not be used with type " + BT::typeToString(left),n.getPosition());

		};
		return BT::VOID;

	}
	
	void __compiler::visit_compare_op(const Node& n) {
		/* Visit compare op
		 */
		static std::map<bst::str,BT::Instruction> instructions {
			{"=",BT::BOOL_EQUAL},
			{"<",BT::BOOL_INFERIOR},
			{">",BT::BOOL_SUPERIOR},
			{"<=",BT::BOOL_INF_EQUAL},
			{">=",BT::BOOL_SUP_EQUAL},
		};
		code.push_back(instructions[n.getValue()]);
	}

	BT::ExprType __compiler::visit_comparison(const Node& n) {
		/* Compile a comparison
		 */
		for (size_t i{0}; i < n.getNumberedChildren().size(); i+=3) {
			// get right operand
			BT::ExprType right_rt{visit(n.get(i + 2))};
			// get left operand
			BT::ExprType left_rt{visit(n.get(i))};

			// check if types match
			if (left_rt != right_rt)
				throwInconsistentType(left_rt,right_rt,n.get(0),n.get(i));
			// if op is <,>, <= or >=, check if type is INT
			const Node& op {n.get(i+1)};
			if (op.getValue() != "=" && left_rt != BT::INT)
				throw exc(exc::InvalidOperator,op.getValue() + " can not be used with type " + BT::typeToString(left_rt),op.getPosition());

			// set operator
			visit_compare_op(op);
			// set type of values compared
			code.push_back(left_rt); // we know left_rt match with right_rt

			// if multiple comparison
			if (i) 
				code.push_back(BT::BOOL_AND);

		}
		return BT::BOOL;
	}

	BT::ExprType __compiler::visit_litint(const Node& n) {
		/* Compile a litteral integer
		 */
		constants.push_back(static_cast<int>(n.getValue()));
		appendInstructions(BT::PUSH,BT::CONSTANT,constants.size()-1);
		return BT::INT;
	}

	void __compiler::throwInconsistentType(BT::ExprType t1, BT::ExprType t2, const Position& n1, const Position& n2) {
		/* Throw an inconsistent error
		 */
		bst::str msg1 {BT::typeToString(t1) + " can not be used with " + BT::typeToString(t2)};
		bst::str msg2 {BT::typeToString(t2) + " defined here: "};
		throw iexc(exc::InconsistantType,msg1,n1, msg2, n2);
	}

	void __compiler::throwInconsistentType(BT::ExprType t1, BT::ExprType t2, const Node& n1, const Node& n2) {
		/* overloaded function
		 */
		throwInconsistentType(t1,t2,n1.getPosition(),n2.getPosition());
	}


	void __compiler::visit_variable_declaration(const Node& n) {
		/* Declare a variable
		 * and (optionnaly) set it
		 */
		const bst::str& name{n.get("name").getValue()};
		bst::str type{n.get("type").getValue()};
		// check: already defined ?
		if (symbol_table->contains(name))
			throw iexc(exc::NameAlreadyDefined,"This name has already been defined here:",symbol_table->getPosition(name),"Name defined another time here: ",n.getPosition());

		// find type
		static const std::map<bst::str,BT::ExprType> types{
			{"text",BT::TEXT},
			{"int",BT::INT},
			{"bool",BT::BOOL},
			{"symbol",BT::SYMBOL},
		};
		type.tolower();
		BT::ExprType type_{BT::VOID};
		try {
			type_ = types.at(type);
		} catch (const std::out_of_range& oor){
			throw exc(exc::UnknownType,"Invalid type declared",n.get("type").getPosition());
		}

		// set symbol
		symbol_table->append(name,type_,n.getPosition());
		// optional: set value
		if (n.getNamedChildren().count("value")) {
			BT::ExprType value_rt {visit(n.get("value"))};
			if (value_rt != type_)
				throwInconsistentType(type_,value_rt,n.get("type").getPosition(),n.get("value").getPosition());

			appendInstructions(BT::ASSIGN,symbol_table->getIndex(name));
			symbol_table->markDefined(name);

		}
	}
}
