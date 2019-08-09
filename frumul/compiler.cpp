#include "compiler.h"
#include "tailresult.h"
#include "textto.h"
// Note: do not forget that what is at right should be push in the stack first

namespace frumul {

	/*
	Compiler::Compiler(Symbol& s, const FString& lang) :
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

	__compiler::__compiler (const Node& n, const ExprType& rt, Symbol& s,const FString& nlang) :
		node{n}, return_type{rt}, bytecode{s}, rtc{rt != ET::TEXT}, parent{s}, lang{nlang}, unsafe_args_remainder{s.getMark().afterArgsNumber()}
	{
	}

	ByteCode __compiler::compile() {
		/* Compile the node into a bytecode
		 */
		if (!bytecode) {
			if (return_type != ET::VOID)
				symbol_table->append("",return_type,node.getPosition()); //an empty name can't be set by the user
				
			// manages, if necessary, the verified args
			visitParameters();

			// compilation
			visit(node);
			// inform the bytecode of the number of variables
			bytecode.addVariable(symbol_table->variableNumber());

			if (!rtc) 
				// no value seems to be returned
				throw exc(exc::NoReturnedValue,"Compiler can not be sure that a value will be return.",node.getPosition());
		}
		return bytecode;
	}

	void __compiler::appendInstructions(std::initializer_list<byte> instructions) {
		/* Append instructions to code
		 */
		for (auto& i : instructions)
			code.push_back(i);
	}

	void __compiler::appendInstructions(const ExprType& t) {
		/* Decompose the ExprType into
		 * byte types
		 */ 
		code.push_back(t.getType());
		if (t.isContainer())
			appendInstructions(t.getContained());
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
		appendInstructions(BT::PUSH,ET::CONSTANT,i);
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

	VarSymbol& __compiler::getOrCreateVarSymbol(const Node& var, const ExprType& type) {
		/* Look for a variable. If not found,
		 * create it
		 * return a reference to it
		 */
		const FString& name{var.getValue()};
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

	ExprType __compiler::visit(const Node& n) {
		/* Dispatch the node following
		 * its type
		 * If the function called return void, this one returned the return type expected
		 */
		switch(n.type()) {
			case Node::BASIC_VALUE:		return visit_basic_value(n);
			case Node::BIN_OP:		return visit_bin_op(n);
			case Node::CONDITION:		return visit_condition(n);
			case Node::COMPARISON:		return visit_comparison(n);
			case Node::INDEX_ASSIGNMENT:	return visit_index_assignment(n);
			case Node::LIST:		return visit_list(n);
			case Node::LIST_WITH_INDEX:	return visit_list_with_index(n);
			case Node::LITBOOL:		return visit_litbool(n);
			case Node::LITINT:		return visit_litint(n);
			case Node::LITSYM:		return visit_litsym(n);
			case Node::LITTEXT:		return visit_littext(n);
			case Node::LOOP:		return visit_loop(n);
			case Node::SYMCALL:		return visit_symcall(n);
			case Node::UNARY_OP:		return visit_unary_op(n);
			case Node::UNSAFE_ARG:		return visit_unsafe_arg(n);
			case Node::VAL_TEXT:		return visit_val_text(n);
			case Node::VARIABLE_ASSIGNMENT:	return visit_variable_assignment(n);
			case Node::VARIABLE_DECLARATION:return visit_variable_declaration(n); 
			case Node::VARIABLE_NAME:	return visit_variable_name(n);
			default:
				printl(n);
				assert(false&&"Node not recognized");
		};
		return ET::VOID; // because clang complains
	}

	ExprType __compiler::visit_basic_value(const Node& n,bool add_return) {
		/* Visit basic value
		 * if add_return is true, the function add return at the end
		 * of the call (only for text returning values: that does not affect
		 * the other types of values, which MUST add RETURN)
		 */
		// enter new scope
		++*symbol_table;
		++rtc;

		constexpr int r_index{0}; // return index
		for (const auto& child : n.getNumberedChildren()) {

			ExprType rt{visit(child)};
			if (rt == ET::VOID)
				continue;

			if (return_type == ET::TEXT) {

				// cast
				if (rt != return_type)
					cast(rt,return_type,n,child);

				appendInstructions(
						BT::PUSH,ET::VARIABLE,r_index, // push returned value on the stack
						BT::TEXT_ADD); // add returned to return value
				appendInstructions(BT::ASSIGN,r_index); // assign back to returned value
			} else {

				if (rt != return_type)
					throwInconsistentType(return_type,rt,n,child);

				// set returned value
				appendInstructions(BT::ASSIGN,r_index,BT::RETURN); // assign last elt of stack to return value and return
				// the RETURN instruction must be kept to end the execution of the value at that position
				// certifies that this scope returns a value
				rtc.set(true);
			}

		}
		if (add_return)
			code.push_back(BT::RETURN);

		// return to parent scope
		--*symbol_table;
		--rtc;
		return return_type;
	}

	ExprType __compiler::visit_bin_op(const Node& n) {
		/* Compile an expression x op x
		 * and return the type
		 */
		ExprType right{visit(n.get("right"))};
		switch (right) {
			case ET::INT:
				{
					ExprType t2{visit(n.get("left"))};
                                        if (t2 == ET::TEXT && n.getValue() == "*") {
                                            code.push_back(BT::TEXT_MUL);
                                            return ET::TEXT;
                                        }
                                        else if (t2 != ET::INT)
						throwInconsistentType(ET::INT,t2,n.get("right"),n.get("left"));

					static std::map<FString,BT::Instruction> types{ {"+",BT::INT_ADD},{"-",BT::INT_SUB}, {"*",BT::INT_MUL},{"/",BT::INT_DIV},{"%",BT::INT_MOD}};
					code.push_back(types[n.getValue()]);
					// handling division by zero
					if (n.getValue() == "/" || n.getValue() == "%")
						bytecode.addRuntimeError(exc(exc::DivisionByZero,"It is impossible to divide by zero",n.get("left").getPosition()));

					return ET::INT;
				}
			case ET::TEXT:
				{
					ExprType t2{visit(n.get("left"))};
					if (n.getValue() == "+") {
						if (t2 != ET::TEXT)
							throwInconsistentType(ET::TEXT,t2,n.get("left"),n.get("right"));
						code.push_back(BT::TEXT_ADD);
					}
					else if (n.getValue() == "*" && DEPRECATED) {
						if (t2 != ET::INT)
							throwInconsistentType(ET::TEXT,t2,n.get("left"),n.get("right"));
						code.push_back(BT::TEXT_MUL);
					}
					else
						throw exc(exc::InvalidOperator,n.getValue() + " can not be used with type TEXT", n.getPosition());
					return ET::TEXT;
				}
			case ET::BOOL:
				{
					ExprType t2{visit(n.get("left"))};
					if (t2 != ET::BOOL)
						throwInconsistentType(ET::BOOL,t2,n.get("left"),n.get("right"));
					static std::map<FString,BT::Instruction> types {
						{"&",BT::BOOL_AND},
						{"|",BT::BOOL_OR},
					};
					code.push_back(types[n.getValue()]);

					return ET::BOOL;
				}
			default:
				if (right & ET::LIST && n.getValue() == "+") {
					ExprType t2{visit(n.get("left"))};
					// add two lists together
					if (t2 == right)
						appendInstructions(BT::LIST_ADD);
					else
						throwInconsistentType(right,t2,n.get("left"),n.get("right"));
                                        return t2;

				}
				else
					throw exc(exc::TypeError,n.getValue() + " can not be used with type " + right.toString(),n.getPosition());

		};
		return ET::VOID;

	}
	
	void __compiler::visit_compare_op(const Node& n) {
		/* Visit compare op
		 */
		static std::map<FString,BT::Instruction> instructions {
			{"=",BT::BOOL_EQUAL},
			{"<",BT::BOOL_INFERIOR},
			{">",BT::BOOL_SUPERIOR},
			{"<=",BT::BOOL_INF_EQUAL},
			{">=",BT::BOOL_SUP_EQUAL},
		};
		code.push_back(instructions[n.getValue()]);
	}

	ExprType __compiler::visit_comparison(const Node& n) {
		/* Compile a comparison
		 */
		// positions relative to the first operand
		constexpr int right_operand_pos{2};
		constexpr int operator_pos{1};
		constexpr int next_left_operand{2};

		for (size_t i{0}; i < n.getNumberedChildren().size()-next_left_operand; i+=next_left_operand) {
			const Node& right {n.get(i + right_operand_pos) };
			const Node& left {n.get(i) };
			// get right operand
			ExprType right_rt{visit(right)};
			// get the op
			const Node& op {n.get(i+operator_pos)};
			// if op is <,>, <= or >=, check if type is INT
			if (op.getValue() != "=" && right_rt != ET::INT)
				throw exc(exc::InvalidOperator,op.getValue() + " can not be used with type " + right_rt.toString(),op.getPosition());

			if (right_rt & ET::LIST)
				visit_list_comparison(right,left,right_rt);
			else {
				// get left operand
				ExprType left_rt{visit(left)};

				// check if types match
				if (left_rt != right_rt)
					throwInconsistentType(left_rt,right_rt,n.get(0),n.get(i));

				// set operator
				visit_compare_op(op);
				// set type of values compared
				code.push_back(left_rt); // we know left_rt match with right_rt
			}

			// if multiple comparison
			if (i) 
				code.push_back(BT::BOOL_AND);

		}
		return ET::BOOL;
	}

	ExprType __compiler::visit_condition(const Node& n) {
		/* Compile a conditional statement
		 * No insertInstructions should be used with that
		 * function
		 * Maybe the jump instructions should be set after the whole code
		 * has been created
		 */
		ExprType rt_compar{visit(n.get("comparison"))};
		if (rt_compar != ET::BOOL)
			throw exc(exc::TypeError,"If statement must be followed by an expression returning a bool",n.get("comparison").getPosition());

		appendInstructions(BT::JUMP_FALSE,0,0); // the two zeros will be filled with the adress to go
		// we now keep the size of the code, first to find how many instructions
		// will be jumped if code is false,
		// second to replace the 0 by the 'adress'
		// we don't -1, because it's easier (see below)
		auto ad_index{code.size()};
		// compile the body of the statement
		visit_basic_value(n.get("text"),false);
		// else part
		if (n.getNamedChildren().count("else_text") > 0) {
			// get the last return value
			bool does_if_return{rtc.pop()};
			appendInstructions(BT::JUMP,0,0);
			auto last_instruction_index{code.size()};
			setJump(ad_index,last_instruction_index);
			// we prepare the last jump
			ad_index = last_instruction_index;
			visit_basic_value(n.get("else_text"),false);
			// set the current return value check
			bool does_else_return{rtc.pop()};
			rtc.set(does_if_return && does_else_return);
		}
		else {
			// pop out last check of return value
			rtc.pop();
		}
		// get the size of the code for the second/third time.
		auto last_instruction_index{code.size()};
		
		setJump(ad_index,last_instruction_index);


		return ET::VOID;
	}

	ExprType __compiler::visit_index(const Node& n) {
		/* compile an index of a text
		 * Node expected contains the index, but it's not the
		 * index itself
		 */
		// push index on the stack
		if (visit(n.get(0)) != ET::INT)
			throw exc(exc::TypeError,"The index must be an integer",n.get("index").getPosition());
		// push variable number on the stack
		// (checks have been done before)
		if (n.type() == Node::VARIABLE_NAME) {
			appendAndPushConstant<int>(symbol_table->getIndex(n.getValue()));
			appendInstructions(BT::TEXT_GET_CHAR,ET::VARIABLE);
		} else {
			// litteral text
			int i {bytecode.addConstant(n.getValue())};
			appendAndPushConstant<int>(i);
			appendInstructions(BT::TEXT_GET_CHAR,ET::CONSTANT);
		}
		// runtime error handled here (after instructions are set)
		bytecode.addRuntimeError(exc{exc::IndexError,"Index is over the number of characters in the text",n.get(0).getPosition()});

		return ET::TEXT;

	}
	
	ExprType __compiler::visit_index_assignment(const Node& n) {
		/* Visit the assignment of an element of
		 * a list or a string
		 * expects a node with numbered children.
		 * first one is the index, last one the
		 */
		const FString& name {n.getValue()};
		const NodeVector& fields{n.getNumberedChildren()};

		checkVariable(name,n); // variable checks
		// type of the variable
		ExprType var_type{symbol_table->getType(name)};
		ExprType type_expected{ET::VOID};

		if (var_type.isConst())
			throw iexc(exc::ValueError,"Variable declared const here: ",symbol_table->getPosition(name),"Can not change value.", n.getPosition());

		if (var_type != ET::TEXT && !(var_type & ET::LIST))
			throw exc(exc::TypeError,"This type can not be used with indices",n.getPosition());

		bool is_char_to_set{false}; // useful for list containing chars to set

		if (var_type == ET::TEXT) {
			type_expected = ET::TEXT;
			if (fields.size() > 2)
				throw iexc(exc::IndexError,"Number of indices can not exceed one for a text",n.getPosition(),"text defined here: ",symbol_table->getPosition(name));
		}
		else if (var_type.getPrimitive(ET::LIST) == ET::TEXT &&
			fields.size() -1 > static_cast<unsigned int>(var_type.getDepth())
			) // case of a list of texts whose last index refers
			// to a char and not a text
		{
			type_expected = ET::TEXT;
			is_char_to_set = true;

			if (fields.size() -2 > static_cast<unsigned int>(var_type.getDepth(ET::LIST))) //(fields.size() - 2 > var_type / ET::LIST)
				throw iexc(exc::IndexError,"Number of indices exceeds the depth of the list",n.getPosition(),"List defined here: ",symbol_table->getPosition(name));
		}
		else {
			if (fields.size() -1 > static_cast<unsigned int>(var_type.getDepth(ET::LIST)))//(fields.size() - 1 > var_type / ET::LIST)
				throw iexc(exc::IndexError,"Number of indices exceeds the depth of the list",n.getPosition(),"List defined here: ",symbol_table->getPosition(name));
			/*type_expected = static_cast<ExprType>(var_type - 
					(ET::LIST * (fields.size()-1))); // BUG overflow when error
			*/
			type_expected = var_type.getContained();
		}

		// push the value first
		const auto& value {fields[negative_index(-1,fields.size())]};
		ExprType val_rt {visit(value)};
		if (val_rt != type_expected)
			cast(val_rt,type_expected,value,n);

		int indices_nb {0}; // useful for list indices
		if (var_type == ET::TEXT) {
			// push the index in second
			if (visit(fields[0]) != ET::INT)
				throw exc(exc::TypeError,"Index must be an integer",fields[0].getPosition());
		} else {
			// get the indices and increment indices_nb
			for (size_t i{0}; i < fields.size() -1; ++i, ++indices_nb) {
				if (visit(fields[i]) != ET::INT)
					throw exc(exc::TypeError,"Index must be an integer",fields[i].getPosition());
			}
		}

		// push the reference of the string/list in last
		appendAndPushConstant(symbol_table->getIndex(name));

		// set the instruction
		if (var_type == ET::TEXT) {
			appendInstructions(BT::TEXT_SET_CHAR);
			bytecode.addRuntimeError(exc{exc::ValueError,"The text contains more than one char",value.getPosition()});
			bytecode.addRuntimeError(exc{exc::IndexError,"Index is over the number of characters in the text",n.getPosition()});
		}
		else {
			appendInstructions(BT::LIST_SET_ELT,indices_nb,is_char_to_set);
			if (is_char_to_set)
				bytecode.addRuntimeError(exc{exc::IndexError,"Index is over the number of elements in the list",n.getPosition()});
			else
				bytecode.addRuntimeError(exc{exc::IndexError,"Index is over the number of elements in the list/text",n.getPosition()});
		}


		return ET::VOID;
	}

	ExprType __compiler::visit_list(const Node& n) {
		/* Visit a litteral list
		 * and compile it
		 * List can be empty.
		 */

		// get the type of the elements
		appendAndPushConstAnyVector();
                if (n.size() == 0) {
                    // empty list
                    return ExprType(ET::LIST,ET::VOID);
                }
		ExprType elt_type{ visit(n.get(0)) };
		appendInstructions(BT::LIST_APPEND);

		for (unsigned int i{1}; i < n.size();++i) {
			if (visit(n.get(static_cast<int>(i))) != elt_type)
				throw iexc(exc::InconsistantType,FString(i) + " element is of different type than the previous elements of the list. First element: ",n.get(0).getPosition(),"Element of other type",n.get(static_cast<int>(i)).getPosition());
			appendInstructions(BT::LIST_APPEND);
		}

		//assert(((elt_type + ET::LIST) < 256 )&& "Maximum list depth is 15 in this implementation"); // clang has a warning for that. Remove it?

		//return static_cast<ExprType>(elt_type + ET::LIST); // set the depth of the list by adding it
		return ExprType(ET::LIST,elt_type);
	}

	ExprType __compiler::visit_list_comparison(const Node& right,const Node& left, const ExprType& right_rt) {
		/* Manages the comparison between two lists, right and left.
		 * The right list must have been compiled before entering
		 * this function
		 * The sign is supposed to be '='.
		 */
		// save the right list in a temporary variable
		VarSymbol& right_id {symbol_table->append(SymbolTab::next(),right_rt,right.getPosition())};
		appendInstructions(BT::ASSIGN,right_id.getIndex());
		// compile the left list
		ExprType left_rt {visit(left)};
		// save the left list in a temporary variable
		VarSymbol& left_id { symbol_table->append(SymbolTab::next(),left_rt,left.getPosition())};
		appendInstructions(BT::ASSIGN,left_id.getIndex());
		// check if types match
		if (left_rt != right_rt)
			throwInconsistentType(left_rt,right_rt,left,right);

		compare_lists(right_id,left_id);
		
		return ET::BOOL;
	}

	ExprType __compiler::compare_lists(VarSymbol& right_id, VarSymbol& left_id) {
		/* Compare two lists saved in right_id and left_id.
		 * Return a vector of sources to jump to the same target
		 */
		const ExprType& type{ right_id.getType()};
		// length of right list
		appendInstructions(BT::PUSH,ET::VARIABLE,right_id.getIndex(),
				BT::LENGTH,ET::LIST);

		// length of the left list + comparison of the lengths
		appendInstructions(BT::PUSH,ET::VARIABLE,left_id.getIndex(),
				BT::LENGTH,ET::LIST,
				BT::BOOL_EQUAL,ET::INT,
				BT::JUMP_FALSE,0,0);
		auto jump_after_len_pos{code.size()};

		// get the length (bis repetita placent)
		VarSymbol& step { symbol_table->append(SymbolTab::next(),ET::INT,right_id.getPosition()) };
		appendAndPushConstant<int>(1);
		appendInstructions(BT::PUSH,ET::VARIABLE,right_id.getIndex(),
				BT::LENGTH,ET::LIST,
				BT::INT_SUB, // length -1
				BT::ASSIGN,step.getIndex());
		// set the loop
		auto pos_before_condition{code.size()};
		appendAndPushConstant<int>(0);	
		appendInstructions(BT::PUSH,ET::VARIABLE,step.getIndex(),
				BT::BOOL_INFERIOR,ET::INT,
				BT::JUMP_TRUE,0,0); // if index == 0, we jump to the end of the loop
		auto jump_after_condition{code.size()};

		if (type.getContained() & ET::LIST) {
			VarSymbol& nright_id { symbol_table->append(SymbolTab::next(),type.getContained(),right_id.getPosition()) };
			VarSymbol& nleft_id { symbol_table->append(SymbolTab::next(),type.getContained(),left_id.getPosition()) };
			// push each element on the stack
			appendInstructions(BT::PUSH,ET::VARIABLE,right_id.getIndex(),
					BT::PUSH,ET::VARIABLE,step.getIndex(),
					BT::LIST_GET_ELT,
					BT::ASSIGN,nright_id.getIndex(),

					BT::PUSH,ET::VARIABLE,left_id.getIndex(),
					BT::PUSH,ET::VARIABLE,step.getIndex(),
					BT::LIST_GET_ELT,
					BT::ASSIGN,nleft_id.getIndex());
			compare_lists(nright_id,nleft_id);
		} else {
			// push each element on the stack
			appendInstructions(BT::PUSH,ET::VARIABLE,right_id.getIndex(),
					BT::PUSH,ET::VARIABLE,step.getIndex(),
					BT::LIST_GET_ELT);
			appendInstructions(BT::PUSH,ET::VARIABLE,left_id.getIndex(),
					BT::PUSH,ET::VARIABLE,step.getIndex(),
					BT::LIST_GET_ELT);
			// compare each element
			appendInstructions(BT::BOOL_EQUAL,type.getContained());
		}
		appendInstructions(BT::JUMP_FALSE,0,0);
		auto jump_after_comparison{code.size()};
		// jump to the start of the loop
		appendAndPushConstant<int>(1);
		appendInstructions(BT::PUSH,ET::VARIABLE,step.getIndex(),
				BT::INT_SUB,
				BT::ASSIGN,step.getIndex(),
				BT::JUMP,0,0);
		setJump(code.size(),pos_before_condition);
		setJump(jump_after_len_pos,code.size());
		setJump(jump_after_comparison,code.size());
		// set the false case
		appendAndPushConstant<bool>(false);
		appendInstructions(BT::JUMP,0,0); // jump after the true case
		auto jump_after_false_case{ code.size() };
		// set the true case
		setJump(jump_after_condition,code.size());
		appendAndPushConstant<bool>(true);
		setJump(jump_after_false_case,code.size());
		return ET::BOOL;
	}

	ExprType __compiler::visit_list_with_index(const Node& n) {
		/* Compile a litteral list or a name followed by an index
		 */
		// we iterates over the map to keep the order of insertion (see parser)
		// TODO this should be refactored with a multimap with std::variant (when possible)


		// load list and get her type
		auto it{n.getNumberedChildren().begin()};
		assert((it->type() == Node::LIST || it->type() == Node::VARIABLE_NAME)&&"First node is not a list");
		ExprType list_type{visit(*it++)}; // we increment AFTER the derefencement

		// iterates over the indices by order
		for (;it != n.getNumberedChildren().end();++it) {
			// check that the number of indices is under the depth of the list
			//if (list_type < ET::LIST && list_type != ET::TEXT) {
			if (list_type.getDepth(ET::LIST) == 0 && list_type != ET::TEXT) {
			
				throw exc(exc::IndexError,"Number of indices is too large for the required list",it->getPosition());
			}

			// push index on the stack
			if (visit(*it) != ET::INT)
				throw exc(exc::TypeError,"Index must be an int",it->getPosition());
			if (list_type == ET::TEXT) {
				// append instructions to get the character
				appendInstructions(BT::TEXT_GET_CHAR,ET::STACK_ELT);
				// add runtime error
				bytecode.addRuntimeError(exc{exc::IndexError,"Index is over the number of characters in the text",it->getPosition()});
			} else {
				// append instructions to get the element
				appendInstructions(BT::LIST_GET_ELT);
				bytecode.addRuntimeError(exc{exc::IndexError,"Index is over the number of elements in the list",it->getPosition()});
				
				//list_type = static_cast<ExprType>(list_type - ET::LIST);
				list_type = list_type.getContained();
			}

		}
		return list_type; // should match with the type of the element extracted

	}

	ExprType __compiler::visit_litbool(const Node& n) {
		/* Return true or false
		 */
		static const std::map<FString,bool> bools{
			{"false",false},
			{"true",true}};
		appendAndPushConstant<bool>(bools.at(n.getValue()));
		return ET::BOOL;
	}
			

	ExprType __compiler::visit_litint(const Node& n) {
		/* Compile a litteral integer
		 */
		appendAndPushConstant<int>(
					//static_cast<int>(n.getValue())
					text_to<int>(n.getValue())
				);
		return ET::INT;
	}

	ExprType __compiler::visit_litsym(const Node& n) {
		/* compile a litteral symbol
		 */
		try {
			RSymbol s{parent.getChildren().find(n.getValue(),Schildren::Relative).getSymbol()};
			appendAndPushConstant<RSymbol>(s);
			return ExprType(ET::SYMBOL,s.get().getReturnType());

		} catch (const FString& path) { // if error
			throw exc(exc::NameError,FString("Name not found: ") + n.getValue(),n.getPosition());
		}
	}

	ExprType __compiler::visit_loop(const Node& n) {
		/* Compile a loop
		 */
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
				case ET::BOOL:
					// nothing to do, since it's the basic case
					break;
				case ET::INT: {
					has_int_variable = true;
					// append a zero constant (if necessary)
					int index_of_zero {bytecode.addConstant(0)};
					// create a hidden variable
					 VarSymbol *v_s = &symbol_table->append(SymbolTab::next(),ET::INT,n.get("condition").getPosition());
					 hidden_variable_i = static_cast<unsigned int>(v_s->getIndex());
					 //appendAndPushConstant<int>(v_s->getIndex());
					 appendInstructions(BT::ASSIGN,hidden_variable_i);
					 //v_s->markDefined();
					 // change the start of loop
					 start_of_loop = code.size();
					 // set the condition
					 appendPushConstant(index_of_zero);
					 appendInstructions(BT::PUSH,ET::VARIABLE,hidden_variable_i,// push custom variable on the stack
							 BT::BOOL_SUPERIOR,ET::INT);

					      }
					break;
				default:
					throw exc(exc::TypeError,"This type can not be used to loop",n.get("condition").getPosition());
			};
		}
		else {
			ExprType variable_filler {visit(n.get("variable_filler"))}; 
			switch (variable_filler) {
				case ET::INT: {
					has_int_variable= true;
					VarSymbol* v_s = &getOrCreateVarSymbol(n.get("variable"),ET::INT);
					hidden_variable_i = static_cast<unsigned int>(v_s->getIndex());
					appendInstructions(BT::ASSIGN,hidden_variable_i);
					//v_s->markDefined();

					int index_of_zero {bytecode.addConstant(0)};
					start_of_loop = code.size();
					appendPushConstant(index_of_zero);
					appendInstructions(BT::PUSH,ET::VARIABLE,hidden_variable_i,
							BT::BOOL_SUPERIOR,ET::INT);
					      }
					break;
				case ET::TEXT:
					{
					has_iterable_variable = true;
					// create variable if necessary
					VarSymbol* v_s = &getOrCreateVarSymbol(n.get("variable"),ET::TEXT);
					hidden_variable_i = static_cast<unsigned int>(v_s->getIndex());
					//v_s->markDefined();
					// create hidden variable to save the index
					VarSymbol* hidden_index = &symbol_table->append(SymbolTab::next(),ET::TEXT,n.getPosition());
					hidden_index_i = static_cast<unsigned int>(hidden_index->getIndex());
					const int index_of_zero{bytecode.addConstant(0)};
					const size_t steps{
						insertInstructions(start_of_loop,
							BT::PUSH,ET::CONSTANT,index_of_zero,
							BT::ASSIGN,hidden_index_i)
					};
					// change start of loop, set just before the push of the text
					start_of_loop += steps;
					// get length of text
					appendInstructions(BT::LENGTH,ET::TEXT);
					// get index value
					appendInstructions(BT::PUSH,ET::VARIABLE,hidden_index_i);
					// set comparison 
					appendInstructions(BT::BOOL_INFERIOR,ET::INT);

					}
					break;
				default:
					if (variable_filler != ET::LIST)
						exc(exc::TypeError,"This type can not be used with a variable",n.get("variable_filler").getPosition());
					
					// case of a list
					// No, I did not copy/paste. I rewrote it by hand. Yes, it's as stupid; maybe even worse. Too lazy to create a function for that.
					{
						//ExprType elt_type{static_cast<ExprType>(variable_filler - ET::LIST)};
						ExprType& elt_type{variable_filler.getContained()};
						has_iterable_variable = true;
						// create variable if necessary
						VarSymbol* v_s = &getOrCreateVarSymbol(n.get("variable"),elt_type);
						hidden_variable_i = static_cast<unsigned int>(v_s->getIndex());
						//v_s->markDefined();
						// create hidden variable to save the index
						VarSymbol* hidden_index = &symbol_table->append(SymbolTab::next(),elt_type,n.getPosition());

						hidden_index_i = static_cast<unsigned int>(hidden_index->getIndex());
						const int index_of_zero{bytecode.addConstant(0)};
						const size_t steps{
							insertInstructions(start_of_loop,
								BT::PUSH, ET::CONSTANT,index_of_zero,
								BT::ASSIGN,hidden_index_i)
						};
						// change start of loop, just before the push of the elements
						start_of_loop += steps;
						// get length of list
						appendInstructions(BT::LENGTH,ET::LIST);
						// get index value
						appendInstructions(BT::PUSH,ET::VARIABLE,hidden_index_i);
						// set comparison
						appendInstructions(BT::BOOL_INFERIOR,ET::INT);
					}
			};
		}
		// add the first jump
		appendInstructions(BT::JUMP_FALSE,0,0); // 0,0 will be filled later
		auto condition_pos{code.size()};
		// manage iterable (list or text) (must be before the body)
		if (has_iterable_variable) {
			// push again text 
			ExprType variable_filler{visit(n.get("variable_filler"))};
			// get index
			appendInstructions(BT::PUSH,ET::VARIABLE,hidden_index_i);
			// push char/list elt on stack and assign it to variable (no runtime error possible here)
			if (variable_filler == ET::TEXT)
				appendInstructions(BT::TEXT_GET_CHAR,ET::STACK_ELT,
						BT::ASSIGN,hidden_variable_i);
			else
				appendInstructions(BT::LIST_GET_ELT,
						BT::ASSIGN,hidden_variable_i);
			// set new index again for next iteration
			appendInstructions(BT::PUSH,ET::VARIABLE,hidden_index_i);
			appendAndPushConstant<int>(1);
			appendInstructions(BT::INT_ADD,
					BT::ASSIGN,hidden_index_i);

		}
		// fill the body
		visit_basic_value(n.get("inside_loop"),false);
		// manage hidden variable (must be after the body)
		if (has_int_variable) {
			appendAndPushConstant<int>(1);
			appendInstructions(BT::PUSH,ET::VARIABLE,hidden_variable_i,
					BT::INT_SUB);
			appendInstructions(BT::ASSIGN,hidden_variable_i);
		}
		// add the second jump
		appendInstructions(BT::JUMP,0,0); // 0,0 will be filled later
		auto second_jump_pos{code.size()};
		// set the jumps target
		setJump(condition_pos,second_jump_pos);
		setJump(second_jump_pos,start_of_loop);
		// return value check has been set,
		// so we need to pop it now
		rtc.pop();
		return ET::VOID;
	}

	ExprType __compiler::visit_littext(const Node& n) {
		/* Compile a litteral text
		 */
		if (n.has(0))
			return visit_index(n);
		appendAndPushConstant<FString>(n.getValue());
		return ET::TEXT;
	}

	void __compiler::throwInconsistentType(const ExprType& t1, const ExprType& t2, const Position& n1, const Position& n2) {
		/* Throw an inconsistent error
		 */
		FString msg1 {t1.toString() + " can not be used with " + t2.toString()};
		FString msg2 {t2.toString() + " defined here: "};
		throw iexc(exc::InconsistantType,msg1,n1, msg2, n2);
	}

	void __compiler::throwInconsistentType(const ExprType& t1, const ExprType& t2, const Node& n1, const Node& n2) {
		/* overloaded function
		 */
		throwInconsistentType(t1,t2,n1.getPosition(),n2.getPosition());
	}

	ExprType __compiler::visit_symcall(const Node& n) {
		/* Compile the call to a symbol
		 */
		struct TypeBool {
			/* This a very special struct
			 * used only inside this function
			 * after compiling an argument, it saves
			 * its type and wether or not the arg
			 * is named
			 * it is later used when setting the code
			 * The position is also given here
			 */
			ExprType type;
			bool named;
			const Position& pos;
		};

		// push elements on stack
		// // visit the name, litteral or reference, and get the type expected
		ExprType symbol_type {visit(n.get("name"))};

		// // manage the arguments
		// // // checks
		const Node& arguments {n.get("arguments")};
		std::vector<TypeBool> arg_keepsake;

		constexpr unsigned int max_arguments_possible{256};
		if (arguments.size() > max_arguments_possible)
			throw exc(exc::ArgumentNBError,FString("Max number of arguments is ") + max_arguments_possible,arguments.getPosition());

		for (auto it{arguments.rbegin()}; it != arguments.rend(); ++it) {
			const bool is_arg_named{ it->type() == Node::NAMED_ARG};
			// push value and get the type
			ExprType type { 
				is_arg_named ? visit(it->get("value")) : visit(*it) 
				};
			// push name on stack if necessary
			if (is_arg_named) {
				appendAndPushConstant<FString>(it->get("name").getValue());
                        }

			arg_keepsake.push_back({type,is_arg_named,it->getPosition()});
		}

		// set the code

		appendInstructions(BT::CALL,arguments.getNumberedChildren().size());
		// iterate over arguments
		for (auto it{arg_keepsake.rbegin()}; it != arg_keepsake.rend();++it) {
                        const auto& elt = *it;
			// set type
			appendInstructions(elt.type);
			// set name instruction
			appendInstructions(static_cast<byte>(elt.named));
			// get the position
			bytecode.setEltPosition(elt.pos);


		}


		// add runtime errors
		// Arguments number
		bytecode.addRuntimeError(exc(exc::ArgumentNBError,"Arguments number does not match the number required",arguments.getPosition()));
		// lang error
		bytecode.addRuntimeError(exc(exc::LangError,"Language requested not found for this value",n.getPosition()));

		return symbol_type.getContained();
	}

	ExprType __compiler::visit_unary_op(const Node& n) {
		/* Compile a unary expression: -,+,!
		 */
		ExprType rt{visit(n.get("expr"))};
		static const std::map<FString,BT::Instruction> instructions {
			{"+",BT::INT_POS},
			{"-",BT::INT_NEG},
			{"!",BT::BOOL_NOT},
		};
		BT::Instruction ins{instructions.at(n.getValue())};
		// check compatibility
		if ((ins == BT::INT_POS || ins == BT::INT_NEG) && rt != ET::INT)
			throw exc(exc::InconsistantType,"Unary operator +/- must be used with integers only",n.getPosition());
		else if (ins == BT::BOOL_NOT && rt != ET::BOOL)
			throw exc(exc::InconsistantType,"Unary operator ! must be used with booleans only",n.getPosition());

		code.push_back(ins);
		return rt;
	}

	ExprType __compiler::visit_unsafe_arg(const Node& n) {
		/* Compile an unsafe argument
		 */
		if (unsafe_args_remainder <= 0)
			throw iexc(exc::ArgumentNBError,"Too many calls of unsafe arguments. Call in excess: ",n.getPosition(),"Number defined here: ", parent.getMark().getPositions());

		int index = parent.getMark().afterArgsNumber() - unsafe_args_remainder;
		
		--unsafe_args_remainder;

		appendInstructions(BT::PUSH,ET::VARIABLE,symbol_table->getIndex(unsafe_name+index));

		return ET::TEXT;
	}

	ExprType __compiler::visit_val_text(const Node& n) {
		/* Append text to the return value
		 */
		// if the value is empty, skip it
		if (!n.getValue())
			return ET::VOID;

		constants.push_back(n.getValue());
		appendInstructions(BT::PUSH,ET::CONSTANT,constants.size()-1);
		return ET::TEXT;
	}

	ExprType __compiler::visit_variable_assignment(const Node& n) {
		/* Assign a value to a variable
		 */
		const FString& name {n.get("name").getValue()};
		if (!symbol_table->contains(name))
			throw exc(exc::NameError,"Name not defined",n.get("name").getPosition());

		ExprType rt{visit(n.get("value"))};
		const ExprType s_type {symbol_table->getType(name)};
		if (rt != s_type && !(rt == ExprType(ET::LIST,ET::VOID) && s_type & ET::LIST)){
			// cast if possible
			cast(rt,s_type,n,n.get("value"));
		}
		if (s_type.isConst())
			throw iexc(exc::ValueError,"Variable declared const here: ",symbol_table->getPosition(name),"Can not change value.", n.getPosition());

		appendInstructions(BT::ASSIGN,symbol_table->getIndex(name));
		return ET::VOID;
	}

	ExprType __compiler::visit_variable_declaration(const Node& n) {
		/* Declare a variable
		 * and set it
		 */
		const FString& name{n.get("name").getValue()};
		FString type{n.get("type").getValue()};
		// check: already defined ?
		if (symbol_table->contains(name,true)) // true for current scope only
			throw iexc(exc::NameAlreadyDefined,"This name has already been defined here:",symbol_table->getPosition(name),"Name defined another time here: ",n.getPosition());

		// find type
		ExprType type_{ExprType(n.get("type"))};

		size_t source_ad{type_.isStatic() ? prepare_static_initialization(n) : 0 };

		// compile value 
		if (n.getNamedChildren().count("value")) {
			ExprType value_rt {visit(n.get("value"))};
			if (value_rt != type_ && !(value_rt == ExprType(ET::LIST,ET::VOID) && type_ & ET::LIST))
                                // we cast only if the value is not an empty list assigned to a list
				cast(value_rt,type_,n,n.get("value"));
		}

		// set symbol
		// We set the symbol after, because if we set it before the assignment, the user can make a call to the variable before this variable has been set
		auto& var { symbol_table->append(name,type_,n.getPosition()) };
		if (type_.isStatic())
			bytecode.addStaticVar(static_cast<unsigned int>(var.getIndex()));

		// assign value
		if (n.getNamedChildren().count("value"))
			appendInstructions(BT::ASSIGN,symbol_table->getIndex(name));
		
		if (type_.isStatic()) // set the jump to skip the initialization after the first assignment
			setJump(source_ad,code.size());

		return ET::VOID;
	}

	ExprType __compiler::visit_list_type_declaration(const Node& n,const ExprType& primitive) {
		/* Return the right type of a list
		 * DEPRECATED
		 */
		ExprType l_type{primitive};
		if (n.has("list_depth") && n.get("list_depth").getValue() != "1") 
			l_type = static_cast<ExprType>(ET::VOID // just for compilation
					//l_type +  static_cast<int>(n.get("list_depth").getValue()) * ET::LIST
					);
		else
			l_type = ET::VOID; // just for compilation
			//l_type = static_cast<ExprType>(l_type + ET::LIST);

		return l_type;

	}

	ExprType __compiler::visit_variable_name(const Node& n) {
		/* compile a call to a variable
		 */
		const FString& name {n.getValue()};
		// checks
		checkVariable(name,n);
		// with index
		if (n.has(0)) {
			if (symbol_table->getType(name) == ET::TEXT)
				return visit_index(n);
			else if (symbol_table->getType(name) & ET::LIST) {
				// modify the node to match with node expected
				// by visit_list_with_index
				NodeVector fields;
				fields.push_back(Node(Node::VARIABLE_NAME,n.getPosition(),name));
				for (const auto& elt : n.getNumberedChildren())
					fields.push_back(elt);

				return visit_list_with_index(Node{Node::LIST_WITH_INDEX,n.getPosition(),fields});
			}
			else
				throw exc(exc::TypeError,"This type can not be used with indices",n.getPosition());
		}
		// with no index
		appendInstructions(BT::PUSH,ET::VARIABLE,symbol_table->getIndex(name));
                // with extension
                if (n.has("extension")) {
                    visit(n.get("extension"));
                }
		
		return symbol_table->getType(name);
	}

	void __compiler::cast(const ExprType& source, const ExprType& target, const Node& source_node, const Node& target_node) {
		/* Compiles a cast if possible between source and target
		 * Cast can be compiled only in assignment, declaration and addition
		 * to the return value if its type is text
		 */

		if (
				source == ET::SYMBOL ||
				(target & ET::SYMBOL && source != ET::TEXT) || // &: because we consider beyond the real type of the target if it is a symbol
				source == ET::VOID ||
				source == ET::LIST ||
				target == ET::LIST
		   )
			throwInconsistentType(target,source,target_node,source_node);

		appendInstructions(BT::CAST,source,target);
		// special case of symbol: we add the types in order to check at runtime that
		// the types match
		if (target & ET::SYMBOL) {
			const ExprType* temp_type{&target.getContained()};
			do {
				appendInstructions(*temp_type);
				if (temp_type->isContainer())
					temp_type = &temp_type->getContained();
				else
					temp_type = nullptr;
			} while (temp_type);
			
			// add runtime errors
			bytecode.addRuntimeError(exc(exc::TypeError,"Symbol return type does'nt match with the type expected",source_node.getPosition()));
			bytecode.addRuntimeError(exc(exc::NameError,"Impossible cast: name not recognized",source_node.getPosition()));
		}

		// add a runtime error
		else if (target != ET::TEXT && source != ET::BOOL) {
			bytecode.addRuntimeError(exc(exc::CastError,"Impossible to cast value",source_node.getPosition()));
		}
	}

	void __compiler::checkVariable(const FString& name, const Node& n) {
		/* Checks that variable name has already
		 * been defined 
		 */
		if (!symbol_table->contains(name))
			throw exc(exc::NameError,"Name not defined",n.getPosition());
		/*if (!symbol_table->isDefined(name)) // DEPRECATED, as variables are always defined at declaration
			throw exc(exc::ValueError,"Variable contains no value",n.getPosition());
			*/
	}

	void __compiler::visitParameters() {
		/* Compiles the parameters
		 */
		constexpr int global_scope {0};
		for (auto& parm : parent.getParameters()) {

			symbol_table->append(parm.getName(),
					(parm.getMax(lang) == 1 ? parm.getType() : ExprType(ET::LIST,parm.getType())),
					parm.getPositions()[0], // we store only the first position
					global_scope);


		}
	}

	size_t __compiler::prepare_static_initialization(const Node& n) {
		/* Prepare the initialization of a static variable
		 */
		// create static bool
		VarSymbol& bool_ {symbol_table->append(symbol_table->next(),ET::BOOL,n.getPosition())};
		bytecode.addStaticVar(static_cast<unsigned int>(bool_.getIndex()),false);
		// push bool on stack
		appendInstructions(BT::PUSH,ET::VARIABLE,bool_.getIndex(),
		// prepare the jump
				BT::JUMP_TRUE,0,0);
		auto ad_index { code.size() };
		// set the new value of the bool -> true
		appendAndPushConstant<bool>(true);
		appendInstructions(BT::ASSIGN,bool_.getIndex());

		return ad_index;
	}

#if 0
	void __compiler::addUnsafeArgsToParms() {
		/* Compiles the unsafe arguments
		 */
		auto& parms { parent.getParameters() };
		auto& pos { parent.getMark().getPositions() };

		for (int i{0}; i < unsafe_args_remainder;++i) {
			parms.push_back(Parameter(unsafe_name + i,ET::TEXT,pos,parent));
		}
	}
#endif


	ByteCode MonoExprCompiler::compile() {
		/* Compiles the node
		 * and return the bytecode
		 */
		if (!bytecode) {
			// add returned value
			symbol_table->append(symbol_table->next(),return_type,node.getPosition());
			constexpr int r_index{0}; // return index
			// compile the expression
			visit(node);
			// affect the value returned by expression to 
			// the returned value
			appendInstructions(BT::ASSIGN,r_index,BT::RETURN);
			// set the number of variables
			bytecode.addVariable(symbol_table->variableNumber());
		}
		return bytecode;
	}

}
