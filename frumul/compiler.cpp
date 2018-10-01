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

	size_t __compiler::insertInstructions(int i, std::initializer_list<byte> instructions) {
		/* insert instructions after i position
		 * of bytecode 
		 * if i is the last element, it can be found with the size of the vector only:
		 * int last_index {code.size()}; // last element of code index
		 */
		std::vector<byte>::iterator begin{bytecode.getBegin()};
		code.insert(begin+i,instructions);
		return instructions.size();
	}

	void __compiler::appendPushConstant(int i) {
		/* Append instructions to push constant on stack
		 * This utility should be called just after added a new constant
		 */
		appendInstructions(BT::PUSH,BT::CONSTANT,i);
	}

	void __compiler::appendAndPushConstAnyVector() {
		/* Special function to create a constant vector
		 * in constants
		 */
		if (_const_vector_pos == -1) {
			constants.push_back(AnyVector{});
			_const_vector_pos = constants.size() -1;
		}
		appendPushConstant(_const_vector_pos);
	}

	void __compiler::setJump(unsigned long source, unsigned long target) {
		/* Set the jump at source position
		 * to point to target
		 */
		// this is the number of instructions to jump.
		// the VM will land on the last instruction of the body
		// of the statement, but this instruction will be skip
		// by the main loop
		// it's important that the variable is not unsigned,
		// since the VM will cast it to a signed one
		int_fast16_t steps{
			static_cast<int_fast16_t>(target - source)
		};
		assert(static_cast<unsigned long>(steps) == target - source && "short is too short (funny, no?)");
		
		auto pair {splitShort(steps)};
		// set the steps just after the jump instruction
		code[source-2] = pair.first;
		code[source-1] = pair.second;
	}

	VarSymbol& __compiler::getOrCreateVarSymbol(const Node& var, BT::ExprType type) {
		/* Look for a variable. If not found,
		 * create it
		 * return a reference to it
		 */
		const bst::str& name{var.getValue()};
		// variable already declared
		if (symbol_table->contains(name)) {
			VarSymbol& v_s{symbol_table->getVarSymbol(name)};
			if (v_s.getType() != type)
				throw iexc(exc::TypeError,"This variable doesn't match the type required", var.getPosition(),"Variable first defined here:",v_s.getPosition());
			return v_s;
		}
		// create new variable
		return symbol_table->append(name,type,var.getPosition());
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
			case Node::LIST:		return visit_list(n);
			case Node::LIST_WITH_INDEX:	return visit_list_with_index(n);
			case Node::LITBOOL:		return visit_litbool(n);
			case Node::LITINT:		return visit_litint(n);
			case Node::LITTEXT:		return visit_littext(n);
			case Node::LOOP:		return visit_loop(n);
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
						BT::TEXT_ADD); // add returned to return value
				//appendAndPushConstant<int>(r_index);
				appendInstructions(BT::ASSIGN,r_index); // assign back to returned value
			} else {

			if (rt != return_type)
				throwInconsistentType(return_type,rt,n,child);

			// set returned value
			//appendAndPushConstant<int>(r_index);
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
			case BT::BOOL:
				{
					BT::ExprType t2{visit(n.get("left"))};
					if (t2 != BT::BOOL)
						throwInconsistentType(BT::BOOL,t2,n.get("left"),n.get("right"));
					static std::map<bst::str,BT::Instruction> types {
						{"&",BT::BOOL_AND},
						{"|",BT::BOOL_OR},
					};
					code.push_back(types[n.getValue()]);

					return BT::BOOL;
				}
			default:
				if (right >= BT::LIST && n.getValue() == "+") {
					BT::ExprType t2{visit(n.get("left"))};
					// add two lists together
					if (t2 == right)
						appendInstructions(BT::LIST_ADD);
					else
						throwInconsistentType(right,t2,n.get("left"),n.get("right"));

				}
				else
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
		// positions relative to the first operand
		constexpr int right_operand_pos{2};
		constexpr int operator_pos{1};
		constexpr int next_left_operand{2};
		printl(n);

		for (size_t i{0}; i < n.getNumberedChildren().size()-next_left_operand; i+=next_left_operand) {
			printl(i);
			// get right operand
			BT::ExprType right_rt{visit(n.get(i + right_operand_pos))};
			// get left operand
			BT::ExprType left_rt{visit(n.get(i))};

			// check if types match
			if (left_rt != right_rt)
				throwInconsistentType(left_rt,right_rt,n.get(0),n.get(i));
			// if op is <,>, <= or >=, check if type is INT
			const Node& op {n.get(i+operator_pos)};
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
		BT::ExprType rt_compar{visit(n.get("comparison"))};
		if (rt_compar != BT::BOOL)
			throw exc(exc::TypeError,"If statement must be followed by an expression returning a bool",n.get("comparison").getPosition());

		appendInstructions(BT::JUMP_FALSE,0,0); // the two zeros will be filled with the adress to go
		// we now keep the size of the code, first to find how many instructions
		// will be jumped if code is false,
		// second to replace the 0 by the 'adress'
		// we don't -1, because it's easier (see beyond)
		auto ad_index{code.size()};
		// compile the body of the statement
		visit_basic_value(n.get("text"),false);
		// else part
		if (n.getNamedChildren().count("else_text") > 0) {
			appendInstructions(BT::JUMP,0,0);
			auto last_instruction_index{code.size()};
			setJump(ad_index,last_instruction_index);
			// we prepare the last jump
			ad_index = last_instruction_index;
			visit_basic_value(n.get("else_text"),false);
		}
		// get the size of the code for the second/third time.
		auto last_instruction_index{code.size()};
		
		setJump(ad_index,last_instruction_index);


		return BT::VOID;
	}

	BT::ExprType __compiler::visit_index(const Node& n, BT::ExprType type) {
		/* compile an index
		 * Node expected contains the index, but it's not the
		 * index itself
		 * TODO manage list
		 */
		// checks
		if (type != BT::TEXT)
			throw exc(exc::TypeError,"This type can not be used with indices",n.getPosition());
		// push index on the stack
		if (visit(n.get("index")) != BT::INT)
			throw exc(exc::TypeError,"The index must be an integer",n.get("index").getPosition());
		// push variable number on the stack
		// (checks have been done before)
		if (n.type() == Node::VARIABLE_NAME) {
			appendAndPushConstant<int>(symbol_table->getIndex(n.getValue()));
			appendInstructions(BT::TEXT_GET_CHAR,BT::VARIABLE);
		} else {
			// litteral text
			int i {bytecode.addConstant(n.getValue())};
			appendAndPushConstant<int>(i);
			appendInstructions(BT::TEXT_GET_CHAR,BT::CONSTANT);
		}

		return BT::TEXT;

	}

	BT::ExprType __compiler::visit_list(const Node& n) {
		/* Visit a litteral list
		 * and compile it
		 * List can not be empty.
		 */

		// get the type of the elements
		appendAndPushConstAnyVector();
		BT::ExprType elt_type{ visit(n.get(0)) };
		appendInstructions(BT::LIST_APPEND);

		for (unsigned int i{1}; i < n.size();++i) {
			if (visit(n.get(i)) != elt_type)
				throw iexc(exc::InconsistantType,bst::str(i) + " element is of different type than the previous elements of the list. First element: ",n.get(0).getPosition(),"Element of other type",n.get(i).getPosition());
			appendInstructions(BT::LIST_APPEND);
		}

		//assert(((elt_type + BT::LIST) < 256 )&& "Maximum list depth is 15 in this implementation"); // clang has a warning for that. Remove it?

		return static_cast<BT::ExprType>(elt_type + BT::LIST); // set the depth of the list by adding it
	}

	BT::ExprType __compiler::visit_list_with_index(const Node& n) {
		/* Compile a litteral list followed by an index
		 */
		// we iterates over the map to keep the order of insertion (see parser)
		// TODO this should be refactored with a multimap with std::variant (when possible)


		// load list and get her type
		auto it{n.getNumberedChildren().begin()};
		assert(it->type() == Node::LIST&&"First node is not a list");
		BT::ExprType list_type{visit(*it++)}; // we increment AFTER the derefencement

		// iterates over the indices by order
		for (;it != n.getNumberedChildren().end();++it) {
			// check that the number of indices is under the depth of the list
			if (list_type < BT::LIST && list_type != BT::TEXT) {
			
				throw exc(exc::IndexError,"Number of indices is too large for the required list",it->getPosition());
			}

			// push index on the stack
			if (visit(*it) != BT::INT)
				throw exc(exc::TypeError,"Index must be an int",it->getPosition());
			if (list_type == BT::TEXT) {
				// append instructions to get the character
				appendInstructions(BT::TEXT_GET_CHAR,BT::STACK_ELT);
			} else {
				// append instructions to get the element
				appendInstructions(BT::LIST_GET_ELT);
				
				list_type = static_cast<BT::ExprType>(list_type - BT::LIST);
			}

		}
		return list_type; // should match with the type of the element extracted

	}

	BT::ExprType __compiler::visit_litbool(const Node& n) {
		/* Return true or false
		 */
		static const std::map<bst::str,bool> bools{
			{"false",false},
			{"true",true}};
		appendAndPushConstant<bool>(bools.at(n.getValue()));
		return BT::BOOL;
	}
			

	BT::ExprType __compiler::visit_litint(const Node& n) {
		/* Compile a litteral integer
		 */
		appendAndPushConstant<int>(
					static_cast<int>(n.getValue())
				);
		return BT::INT;
	}

	BT::ExprType __compiler::visit_loop(const Node& n) {
		/* Compile a loop
		 */
#pragma message "Loops not yet set: list"
		/* This function uses pointers to elements of vector.
		 * This is dangerous, since vectors can be resized and 
		 * the pointers become dangling. Be careful if you
		 * modify the code
		 */

		auto start_of_loop{code.size()};
		// useful with int
		unsigned int hidden_variable_i{0};
		// useful with list and text iteration
		unsigned int hidden_index_i{0};
		bool has_int_variable{false};
		bool has_iterable_variable{false};

		// Which kind of expression follows 'loop' keyword ?
		if (n.getNamedChildren().count("condition") > 0) {
			switch (visit(n.get("condition"))) {
				case BT::BOOL:
					// nothing to do, since it's the basic case
					break;
				case BT::INT: {
					has_int_variable = true;
					// append a zero constant (if necessary)
					int index_of_zero {bytecode.addConstant(0)};
					// create a hidden variable
					 VarSymbol *v_s = &symbol_table->append(SymbolTab::next(),BT::INT,n.get("condition").getPosition());
					 hidden_variable_i = v_s->getIndex();
					 //appendAndPushConstant<int>(v_s->getIndex());
					 appendInstructions(BT::ASSIGN,hidden_variable_i);
					 v_s->markDefined();
					 // change the start of loop
					 start_of_loop = code.size();
					 // set the condition
					 appendPushConstant(index_of_zero);
					 appendInstructions(BT::PUSH,BT::VARIABLE,hidden_variable_i,// push custom variable on the stack
							 BT::BOOL_SUPERIOR,BT::INT);

					      }
					break;
				default:
					throw exc(exc::TypeError,"This type can not be used to loop",n.get("condition").getPosition());
			};
		}
		else {
			switch (visit(n.get("variable_filler"))) {
				case BT::INT: {
					has_int_variable= true;
					VarSymbol* v_s = &getOrCreateVarSymbol(n.get("variable"),BT::INT);
					hidden_variable_i = v_s->getIndex();
					appendInstructions(BT::ASSIGN,hidden_variable_i);
					v_s->markDefined();

					int index_of_zero {bytecode.addConstant(0)};
					start_of_loop = code.size();
					appendPushConstant(index_of_zero);
					appendInstructions(BT::PUSH,BT::VARIABLE,hidden_variable_i,
							BT::BOOL_SUPERIOR,BT::INT);
					      }
					break;
				case BT::TEXT:
					{
					has_iterable_variable = true;
					// create variable if necessary
					VarSymbol* v_s = &getOrCreateVarSymbol(n.get("variable"),BT::TEXT);
					hidden_variable_i = v_s->getIndex();
					v_s->markDefined();
			printl(*v_s);
					// create hidden variable to save the index
					VarSymbol* hidden_index = &symbol_table->append(SymbolTab::next(),BT::TEXT,n.getPosition());
					hidden_index_i = hidden_index->getIndex();
					const int index_of_zero{bytecode.addConstant(0)};
					const size_t steps{
						insertInstructions(start_of_loop,
							BT::PUSH,BT::CONSTANT,index_of_zero,
							BT::ASSIGN,hidden_index_i)
					};
					// change start of loop, set just before the push of the text
					start_of_loop += steps;
					// get length of text
					appendInstructions(BT::LENGTH,BT::TEXT);
					// get index value
					appendInstructions(BT::PUSH,BT::VARIABLE,hidden_index_i);
					// set comparison 
					appendInstructions(BT::BOOL_INFERIOR,BT::INT);

					}
					break;
				default:
					exc(exc::TypeError,"This type can not be used with a variable",n.get("variable_filler").getPosition());
			};
		}
		// add the first jump
		appendInstructions(BT::JUMP_FALSE,0,0); // 0,0 will be filled later
		auto condition_pos{code.size()};
		// manage iterable (list or text) (must be before the body)
		if (has_iterable_variable) {
			// push again text 
			visit(n.get("variable_filler"));
			// get index
			appendInstructions(BT::PUSH,BT::VARIABLE,hidden_index_i);
			// push char on stack and assign it to variable
			appendInstructions(BT::TEXT_GET_CHAR,BT::STACK_ELT,
					BT::ASSIGN,hidden_variable_i);
			// set new index again for next iteration
			appendInstructions(BT::PUSH,BT::VARIABLE,hidden_index_i);
			appendAndPushConstant<int>(1);
			appendInstructions(BT::INT_ADD,
					BT::ASSIGN,hidden_index_i);

		}
		// fill the body
		visit_basic_value(n.get("inside_loop"),false);
		// manage hidden variable (must be after the body)
		if (has_int_variable) {
			appendAndPushConstant<int>(1);
			appendInstructions(BT::PUSH,BT::VARIABLE,hidden_variable_i,
					BT::INT_SUB);
			appendInstructions(BT::ASSIGN,hidden_variable_i);
		}
		// add the second jump
		appendInstructions(BT::JUMP,0,0); // 0,0 will be filled later
		auto second_jump_pos{code.size()};
		// set the jumps target
		setJump(condition_pos,second_jump_pos);
		setJump(second_jump_pos,start_of_loop);
		return BT::VOID;
	}

	BT::ExprType __compiler::visit_littext(const Node& n) {
		/* Compile a litteral text
		 */
		if (n.has("index"))
			return visit_index(n,BT::TEXT);
		appendAndPushConstant<bst::str>(n.getValue());
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
		//appendAndPushConstant<int>(symbol_table->getIndex(name));
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
			{"list",BT::LIST},
		};
		type.tolower();
		BT::ExprType type_{BT::VOID};
		bst::str type_key{"type"}; // useful for the catch beyond
		try {
			type_ = types.at(type);
			// manages list
			if (type_ == BT::LIST) {
				type_key = "primitive_type";
				bst::str primitive_type{n.get(type_key).getValue()};
				primitive_type.tolower();
				type_ = visit_list_type_declaration(n, types.at(primitive_type));
			}
				

		} catch (const std::out_of_range& oor){
			throw exc(exc::UnknownType,"Invalid type declared",n.get(type_key).getPosition());
		}

		// set symbol
		symbol_table->append(name,type_,n.getPosition());
		// optional: set value
		if (n.getNamedChildren().count("value")) {
			BT::ExprType value_rt {visit(n.get("value"))};
			if (value_rt != type_)
				throwInconsistentType(type_,value_rt,n.get("type").getPosition(),n.get("value").getPosition());

			//appendAndPushConstant<int>();
			appendInstructions(BT::ASSIGN,symbol_table->getIndex(name));
			symbol_table->markDefined(name);

		}
		return BT::VOID;
	}

	BT::ExprType __compiler::visit_list_type_declaration(const Node& n,BT::ExprType primitive) {
		/* Return the right type of a list
		 */
		BT::ExprType l_type{primitive};
		if (n.has("list_depth") && n.get("list_depth").getValue() != "1") 
			l_type = static_cast<BT::ExprType>(
					l_type +  static_cast<int>(n.get("list_depth").getValue()) * BT::LIST
					);
		else
			l_type = static_cast<BT::ExprType>(l_type + BT::LIST);

		return l_type;

	}

	BT::ExprType __compiler::visit_variable_name(const Node& n) {
		/* compile a call to a variable
		 */
#pragma message "List not yet set"
		const bst::str& name {n.getValue()};
		// checks
		if (!symbol_table->contains(name))
			throw exc(exc::NameError,"Name not defined",n.getPosition());
		if (!symbol_table->isDefined(name))
			throw exc(exc::ValueError,"Variable contains no value",n.getPosition());

		// with index
		if (n.has("index")) {
			return visit_index(n,symbol_table->getType(name));
		}
		// with no index
		appendInstructions(BT::PUSH,BT::VARIABLE,symbol_table->getIndex(name));
		
		return symbol_table->getType(name);
	}
}
