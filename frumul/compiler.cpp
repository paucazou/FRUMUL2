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
			if (return_type != BT::VOID)
				symbol_table->append("",return_type,node.getPosition()); //an empty name is the only one that can't be set by the user
			visit(node);
			bytecode.addVariable(symbol_table->variableNumber());
			printl("Bytecode:");
			for (const auto& elt : bytecode.getCode())
				printl(static_cast<int>(elt));
		}
		return bytecode;
	}

	void __compiler::appendInstructions(std::initializer_list<byte> instructions) {
		/* Append instructions to code
		 */
		for (auto& i : instructions)
			code.push_back(i);
	}

	void __compiler::insertInstructions(int i, std::initializer_list<byte> instructions) {
		/* insert instructions after i position
		 * of bytecode 
		 * if i is the last element, it can be found with the size of the vector only:
		 * int last_index {code.size()}; // last element of code index
		 */
		std::vector<byte>::iterator begin{bytecode.getBegin()};
		code.insert(begin+i,instructions);
	}

	void __compiler::appendPushLastConstant() {
		/* Append instructions to push constant on stack
		 * This utility should be called just after added a new constant
		 */
		appendInstructions(BT::PUSH,BT::CONSTANT,constants.size()-1);
	}

	BT::ExprType __compiler::visit(const Node& n) {
		/* Dispatch the node following
		 * its type
		 * If the function called return void, this one returned the return type expected
		 */
		switch(n.type()) {
			case Node::BASIC_VALUE:		return visit_basic_value(n);
			case Node::BIN_OP:		return visit_bin_op(n);
			case Node::CONDITION:		return visit_condition(n);
			case Node::COMPARISON:		return visit_comparison(n);
			case Node::LITBOOL:		return visit_litbool(n);
			case Node::LITINT:		return visit_litint(n);
			case Node::LITTEXT:		return visit_littext(n);
			case Node::UNARY_OP:		return visit_unary_op(n);
			case Node::VAL_TEXT:		return visit_val_text(n);
			case Node::VARIABLE_ASSIGNMENT:	return visit_variable_assignment(n);
			case Node::VARIABLE_DECLARATION:return visit_variable_declaration(n);
			case Node::VARIABLE_NAME:	return visit_variable_name(n);
			default:
				printl(n);
				assert(false&&"Node not recognized");
		};
		return BT::VOID; // because clang complains
	}

	BT::ExprType __compiler::visit_basic_value(const Node& n,bool add_return) {
		/* Visit basic value
		 * if add_return is true, the function add return at the end
		 * of the call (only for text returning values: that does not affect
		 * the other types of values, which MUST add RETURN)
		 */
		constexpr int r_index{0}; // return index
		for (const auto& child : n.getNumberedChildren()) {

			//int last_index {static_cast<int>(code.size())}; // last element of code index
			BT::ExprType rt{visit(child)};

			if (return_type == BT::TEXT) {
				if (rt == BT::VOID)
					continue;
				// prepare the stack to append to return value
				//insertInstructions(last_index,BT::PUSH,BT::VARIABLE,r_index);

				// cast
				if (rt == BT::SYMBOL) // cast impossible
					throwInconsistentType(return_type,rt,n,child);
				else if (rt != return_type)
					appendInstructions(BT::CAST,rt,return_type);

				appendInstructions(
						BT::PUSH,BT::VARIABLE,r_index, // push returned value on the stack
						BT::TEXT_ADD, // add returned to return value
						BT::ASSIGN,r_index); // assign back to returned value
			} else {

			if (rt != return_type)
				throwInconsistentType(return_type,rt,n,child);

			// set returned value
			appendInstructions(BT::ASSIGN,r_index,BT::RETURN); // assign last elt of stack to return value and return
			}

		}
		if (add_return)
			code.push_back(BT::RETURN);
		return return_type;
	}

	BT::ExprType __compiler::visit_bin_op(const Node& n) {
		/* Compile an expression x op x
		 * and return the type
		 */
		BT::ExprType right{visit(n.get("right"))};
		switch (right) {
			case BT::INT:
				{
					BT::ExprType t2{visit(n.get("left"))};
					if (t2 != BT::INT)
						throwInconsistentType(BT::INT,t2,n.get("left"),n.get("right"));

					static std::map<bst::str,BT::Instruction> types{ {"+",BT::INT_ADD},{"-",BT::INT_SUB}, {"*",BT::INT_MUL},{"/",BT::INT_DIV},{"%",BT::INT_MOD}};
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
				throw exc(exc::TypeError,n.getValue() + " can not be used with type " + BT::typeToString(right),n.getPosition());

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

	BT::ExprType __compiler::visit_condition(const Node& n) {
		/* Compile a conditional statement
		 * No insertInstructions should be used with that
		 * function
		 * Maybe the jump instructions should be set after the whole code
		 * has been created
		 */
#pragma message "else not set"
		BT::ExprType rt_compar{visit(n.get("comparison"))};
		if (rt_compar != BT::BOOL)
			throw exc(exc::TypeError,"If statement must be followed by an expression returning a bool",n.get("comparison").getPosition());

		appendInstructions(BT::JUMP_FALSE,0,0); // the two zeros will be filled with the adress to go
		// we now keep the size of the code, first to find how many instructions
		// will be jumped if code is false,
		// second to replace the 0 by the 'adress'
		// we don't -1, because it's easier (see above)
		auto ad_index{code.size()};
		// compile the body of the statement
		visit_basic_value(n.get("text"),false);
		// get the size of the code for the second time.
		auto last_instruction_index{code.size()};
		// this is the number of instructions to jump.
		// the VM will land on the last instruction of the body
		// of the statement, but this instruction will be skip
		// by the main loop
		// it's important that the variable is not unsigned,
		// since the VM will cast it to a signed one
		int_fast16_t steps{
			static_cast<int_fast16_t>(last_instruction_index - ad_index)
		};

		assert(static_cast<unsigned long>(steps) == last_instruction_index - ad_index && "short is too short (funny, no?)");

		// split the steps in two bytes
		byte i { static_cast<byte>(steps) }; 		// least important bits
		byte j { static_cast<byte>(steps >> 8) }; 	// most important bits

		// set the bytes in code
		code[ad_index-2] = j;
		code[ad_index-1] = i;


		return BT::VOID;
	}

	BT::ExprType __compiler::visit_litbool(const Node& n) {
		/* Return true or false
		 */
		static const std::map<bst::str,bool> bools{
			{"false",false},
			{"true",true}};
		constants.push_back(bools.at(n.getValue()));
		appendPushLastConstant();
		return BT::BOOL;
	}
			

	BT::ExprType __compiler::visit_litint(const Node& n) {
		/* Compile a litteral integer
		 */
		constants.push_back(static_cast<int>(n.getValue()));
		appendInstructions(BT::PUSH,BT::CONSTANT,constants.size()-1);
		return BT::INT;
	}

	BT::ExprType __compiler::visit_littext(const Node& n) {
		/* Compile a litteral text
		 */
		constants.push_back(n.getValue());
		appendPushLastConstant();
		return BT::TEXT;
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

	BT::ExprType __compiler::visit_unary_op(const Node& n) {
		/* Compile a unary expression: -,+,!
		 */
		BT::ExprType rt{visit(n.get("expr"))};
		static const std::map<bst::str,BT::Instruction> instructions {
			{"+",BT::INT_POS},
			{"-",BT::INT_NEG},
			{"!",BT::BOOL_NOT},
		};
		BT::Instruction ins{instructions.at(n.getValue())};
		// check compatibility
		if ((ins == BT::INT_POS || ins == BT::INT_NEG) && rt != BT::INT)
			throw exc(exc::InconsistantType,"Unary operator +/- must be used with integers only",n.getPosition());
		else if (ins == BT::BOOL_NOT && rt != BT::BOOL)
			throw exc(exc::InconsistantType,"Unary operator ! must be used with booleans only",n.getPosition());

		code.push_back(ins);
		return rt;
	}

	BT::ExprType __compiler::visit_val_text(const Node& n) {
		/* Append text to the return value
		 */
		constants.push_back(n.getValue());
		appendInstructions(BT::PUSH,BT::CONSTANT,constants.size()-1);
		return BT::TEXT;
	}

	BT::ExprType __compiler::visit_variable_assignment(const Node& n) {
		/* Assign a value to a variable
		 */
		const bst::str& name {n.get("name").getValue()};
		if (!symbol_table->contains(name))
			throw exc(exc::NameError,"Name not defined",n.get("name").getPosition());

		BT::ExprType rt{visit(n.get("value"))};
		const BT::ExprType s_type {symbol_table->getType(name)};
		if (rt != s_type) {
			// cast if possible
			if (rt == BT::SYMBOL ||
				(s_type == BT::SYMBOL && rt != BT::TEXT)
			   )
				throwInconsistentType(rt,s_type,n.get("value"),n.get("name"));
			else
				appendInstructions(BT::CAST,rt,s_type);
		}
		appendInstructions(BT::ASSIGN,symbol_table->getIndex(name));
		symbol_table->markDefined(name);
		return BT::VOID;
	}

	BT::ExprType __compiler::visit_variable_declaration(const Node& n) {
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
		return BT::VOID;
	}

	BT::ExprType __compiler::visit_variable_name(const Node& n) {
		/* compile a call to a variable
		 */
#pragma message "List not yet set"
		const bst::str& name {n.getValue()};
		if (!symbol_table->contains(name))
			throw exc(exc::NameError,"Name not defined",n.getPosition());
		if (!symbol_table->isDefined(name))
			throw exc(exc::ValueError,"Variable contains no value",n.getPosition());
		appendInstructions(BT::PUSH,BT::VARIABLE,symbol_table->getIndex(name));
		return symbol_table->getType(name);
	}
}
