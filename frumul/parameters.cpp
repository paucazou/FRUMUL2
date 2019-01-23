#include <climits>
#include <unordered_map>
#include "functions.inl"
#include "parameters.h"
#include "parmqueuer.h"
#include "compiler.h"
#include "symbol.h"
#include "vm.h"

namespace frumul {


	Parameter::Parameter(const Node& node, Symbol& np) :
		Parameter(node,&np)
	{}

	Parameter::Parameter(const Node& node,Symbol* np) : 
		type{node.get("variable").get("type")},
		name{node.get("variable").get("name").getValue()},
		parent{np}
	{
		/* Constructs the base of the parameter
		 */
		assert (node.type() == Node::PARAM&&"Node is not a parameter");
		//type
		/*
		const bst::str& t{node.get("variable").get("type").getValue()};
		if (t == "text")
			type = Text;
		else if (t == "int")
			type = Int;
		else if (t == "bool")
			type = Bool;
		else if (t == "Symbol")
			type = Symbol;
		else
			throw exc(exc::UnknownType,"Unknown type",node.get("variable").get("type").getPosition());
		*/
		const StrNodeMap& fields{node.getNamedChildren()};
		// arg number
		setMinMax(fields);
		// optional
		// // choices
		if (fields.count("choices"))
			choices = std::make_unique<Node>(fields.at("choices"));

		// // default value
		const StrNodeMap& var_fields{node.get("variable").getNamedChildren()};
		if (var_fields.count("value"))
			def = std::make_unique<Node>(var_fields.at("value"));

		// positions
		pos.push_back(node.getPosition());
		if (type.isStatic())
			throw exc(exc::TypeError,"A parameter can not be static",node.getPosition());

	}

	Parameter::Parameter (const bst::str& nname, const ExprType& ntype, const std::vector<Position>& npos,Symbol& nparent) :
		type{ntype},
		name{nname},
		limit1 {std::make_unique<Limit>(Limit(1,Limit::EQUAL))},
		pos{npos},
		parent{&nparent}
	{
		/* Creates a parameter with a limit equal to 1
		 */
	}

	Parameter::Parameter(const Parameter& other) :
		type{other.type},
		name{other.name},
		def{uniq_copy<Node>(other.def)},
		choices{uniq_copy<Node>(other.choices)},
		pos{other.pos},
		parent{other.parent},
		index{other.index}
	{
		/* Copy constructor
		 * Temporary is not copied
		 */
		if (other.limit1)
			limit1 = std::make_unique<Limit>(*other.limit1);
		if (other.limit2)
			limit2 = std::make_unique<Limit>(*other.limit2);

	}

	Parameter::~Parameter () {
		/* Destructor.
		 * TODO it's probably useless now
		 */
		/*
		if (limit1)
			delete limit1;
		if (limit2)
			delete limit2;
			*/
	}

	void Parameter::appendPos(const Position& npos) {
		/* Add a new position (when the user declares two
		 * series of parameters identical
		 */
		pos.push_back(npos);
	}

	void Parameter::setMinMax(const StrNodeMap& fields) {
		/* Try to find the number of args and set min and max
		 * according to the fields entered
		 */
		assert(!limit1&&!limit2&&"Limits have been set already");
		if (fields.count("argnb0")) {
			// at least one parameter
			const Node& arg {fields.at("argnb0")};
			Limit::Comparison c{comparisonValue(arg.getValue())};
			limit1 = std::make_unique<Limit>(
					arg.getNumberedChildren()[0],
					c);
			if (fields.count("argnb1")) {
				const Node& arg {fields.at("argnb1")};
				Limit::Comparison c{comparisonValue(arg.getValue())};
				limit2 = std::make_unique<Limit>(
					arg.getNumberedChildren()[0],
					c);

				// check consistency
				switch(limit1->getComparison()) {
					case Limit::SUPERIOR:
					case Limit::SEQUAL:
					case Limit::EQUAL:
						break;
					case Limit::IEQUAL:
					case Limit::INFERIOR:
						throw exc(exc::ArgumentNBError,"When setting two limits, the only comparison symbols allowed for the first one are >,>= and =",limit1->getPosition());
				};

				switch (limit2->getComparison()) {
					case Limit::IEQUAL:
					case Limit::INFERIOR:
					case Limit::EQUAL:
						break;
					case Limit::SEQUAL:
					case Limit::SUPERIOR:
						throw exc(exc::ArgumentNBError,"When setting two limits, the only comparison symbols allowed for the second one are <,<= and =",limit1->getPosition());

				}
			}
		}
		else {
			// implicit: =1
			limit1 = std::make_unique<Limit>(Limit(1,Limit::EQUAL));
		}


	}


	void Parameter::setParent(Symbol& np) {
		/* Set a new parent
		 */
		parent = &np;
	}

	void Parameter::setIndex(int i) {
		/* Set the index
		 */
		assert(index == -1 && "index already set");
		index = i;
	}

	Parameter::Limit::Comparison Parameter::comparisonValue(const bst::str& val) const{
		/* Return an enum
		 * matching with val
		 */
		if (val == "=")
			return Limit::EQUAL;
		else if (val == ">")
			return Limit::SUPERIOR;
		else if (val == "<")
			return Limit::INFERIOR;
		else if (val == ">=")
			return Limit::SEQUAL;
		else if (val == "<=")
			return Limit::IEQUAL;
		else
			assert(false&&"Unknown binary operator");
	}

	bool Parameter::operator == (const Parameter& other) const {
		/* true if other is strictly equal to *this
		 * (except for the positions)
		 */
		return (type == other.type &&
			name == other.name &&
			limit1 == other.limit1 &&
			limit2 == other.limit2);
	}

	bool Parameter::operator != (const Parameter& other) const {
		/* true if operator == is false
		 */
		return !(*this == other);
	}

	const Node& Parameter::getNodeDefault() const {
		return *def;
	}

	const Node& Parameter::getChoices() const {
		return *choices;
	}

	const ExprType& Parameter::getType() const {
		return type;
	}

	const bst::str& Parameter::getName() const {
		return name;
	}

	int Parameter::getMin(const bst::str& lang) {
		if (min == -1)
			calculateMinMax(lang);
		return min;
	}

	int Parameter::getMax(const bst::str& lang) {
		if (max == -1)
			calculateMinMax(lang);
		return max;
	}

	void Parameter::calculateMinMax(const bst::str& lang) {
		/* Calculate min and max
		 * and returns it, min as first,
		 * and max as second
		 * checks the error (impossible to do this before)
		 */
		assert(parent&&"Parameter: parent is not set");
		assert(min == -1 && "Min already set");
		assert(max == -1 && "Max already set");

		if (limit2) { 
			int min_ { limit1->getLimit(lang,*parent) };
			int max_ {limit2->getLimit(lang,*parent) };

			// limits are equal
			if (min_ == max_) {
				if (limit1->getComparison() != limit2->getComparison())
					throw iexc(exc::ArgumentNBError,"Limits given to the parameter have the same value but not the same sign. Limit 1:",limit1->getPosition(),"Limit 2:",limit2->getPosition());

				return calculateMinMaxWithOneLimit(min_,limit1->getComparison());

			}
			else if (min_ > max_)
				throw iexc(exc::ArgumentNBError,"Min is over max. Min: ",limit1->getPosition(),"Max: ",limit2->getPosition());

			// get real value
			if (limit1->getComparison() == Limit::SUPERIOR)
				++min_;
			if (limit2->getComparison() == Limit::INFERIOR)
				--max_;

			// last checks
			if (min_ < 0)
				throw exc(exc::ValueError,"Limit given is under zero",limit1->getPosition());
			min = min_;
			max = max_;

		}

		return calculateMinMaxWithOneLimit(limit1->getLimit(lang,*parent),limit1->getComparison());
	}

	void Parameter::calculateMinMaxWithOneLimit(int limit, Limit::Comparison c) {
		constexpr int absolute_min {0};
		// check errors
		if (limit<absolute_min)
			throw exc(exc::ValueError,"Limit given is under zero",limit1->getPosition());

		switch (c) {
			case Limit::EQUAL:
				min = limit;
				max = limit;
			case Limit::SUPERIOR:
				min = limit +1;
				max = INT_MAX;
			case Limit::INFERIOR:
				min = absolute_min;
				max = limit-1;
			case Limit::SEQUAL:
				min = limit;
				max = INT_MAX;
			case Limit::IEQUAL:
				min = absolute_min;
				max = limit;

		};
	}

	const PosVect& Parameter::getPositions() const {
		return pos;
	}

	int Parameter::getIndex() const {
		assert(index > -1 && "Index not set");
		return index;
	}

	bool Parameter::operator == (int nb) const {
		/* true if nb is equal to min AND max
		 */
		return (nb == min && nb == max);
	}

	bool Parameter::operator > (int nb) const {
		/* true if max is above nb
		 */
		return max > nb;
	}

	bool Parameter::operator < (int nb) const {
		/* true if min is beyond nb
		 */
		return min < nb;
	}

	bool Parameter::between (int nb) const {
		/* true if nb is between or equal to min
		 * and max
		 */
		assert(min > -1 && max > -1 && "Min and max not set");

		return (nb <= max && nb >= min);
	}

	bool Parameter::between (int nb, const bst::str& lang) {
		/* Can set min and max
		 */
		return (nb <= getMax(lang) && nb >= getMin(lang));
	}

	bool Parameter::hasDefault() const {
		/* true if a default value is set
		 */
		return static_cast<bool>(def);
	}

	bool Parameter::operator == (const bst::str& n) const {
		/* true if n is equal to name
		 */
		return n == name;
	}

	bst::str Parameter::toString() const {
		/* representation of the instance
		 * Return a partial one, in order to be filled
		 * by overloads of this function
		 */
		bst::str s{"<Parameter|"};
		s += type.toString(true) + ">\n"; // true: to avoid unnecessary details
		s += "Name: " + name + "\n";
		if (def)
			s += "Has default value\n";
		if (choices)
			s += "Has choices\n";
		s += "Has ";
		if (limit1) {
			if (limit2)
				s += "2";
			else
				s += "1";
		} else
			s += "0";
		s += " limit(s)\n";
		for (const auto& p : pos)
			s += p.toString();
		return s;
	}

	// Parameter::Limit

	Parameter::Limit::Limit(const Node& n, Limit::Comparison c) :
		comparison{c}, isNode{true}, node{ new Node(n)}, pos{n.getPosition()}
	{
		/* Constructs with a node
		 * yet to be evaluated
		 */
	}

	Parameter::Limit::Limit (int ni, Limit::Comparison c) :
		comparison{c}, isNode{false}, i{ni}, pos{-1,-1,"",""}
	{
		/* Constructs directly an int
		 */
	}

	Parameter::Limit::Limit(const Limit& other):
		comparison{other.comparison}, isNode{other.isNode},
		pos{other.getPosition()}
	{
		if (other.isNode)
			node = new Node(*other.node);
		else
			i = other.i;
	}

	Parameter::Limit::~Limit() {
		/*Destructor
		 */
		if (isNode)
			delete node;
	}

	int Parameter::Limit::getLimit(const bst::str& lang,Symbol& parent) {
		/* Return the limit as an int.
		 * Evaluates the node (if there is a node)
		 * or return the int
		 * the int returned should be compared with the comparison field
		 */
		if (isNode) {
			auto compiler {MonoExprCompiler(*node, ET::INT, parent,lang)};
			auto bt {compiler.compile()};
			auto vm {VM(bt, lang, std::vector<E::any>())};
			delete node;
			i = E::any_cast<int>(vm.run());
			isNode = false;
		}
		return i;
	}

	Parameter::Limit::Comparison Parameter::Limit::getComparison() const {
		return comparison;
	}

	const Position& Parameter::Limit::getPosition() const {
		return pos;
	}

	bst::str Parameter::Limit::toString() const {
		bst::str s { "<Limit> " };
		if (isNode) {
			s += "(Node)\n";
			s += node->toString();
		}
		else {
			s += "(Bytecode)\n";
			s += pos.toString();
		}
		return s;

	}

	bool Parameter::Limit::isConform(int x,const bst::str& lang,Symbol& parent) {
		/* true if x respects the limit
		 */
		int limit {getLimit(lang,parent)};
		switch (comparison) {
			case EQUAL:
				return x == limit;
			case SUPERIOR:
				return limit > x;
			case INFERIOR:
				return limit < x;
			case SEQUAL:
				return limit >= x;
			case IEQUAL:
				return limit <= x;
			default:
				assert(false&&"Limit::Comparison operator not correctly set");
		};
		return false;
	}

	bool Parameter::choiceMatch(const E::any& elt,const bst::str& lang) {
		/* true if elt match one of the elements
		 * of the choices list
		 */
		// if no choice has been set
		if (!_choices && !choices)
			return true;

		// compile expression if necessary
		if (!_choices) {
			auto compiler { MonoExprCompiler(*choices,ExprType(ET::LIST,type),*parent,lang)};
			auto bt { compiler.compile() };
			auto vm { VM(bt, lang, std::vector<E::any>()) };
			_choices = std::make_unique<std::vector<E::any>>(E::any_cast<std::vector<E::any>>(vm.run()) );
			choices.reset();
		}
		// checks that elt match one of _choices
		switch (type) { // this gets only the high level type, not the full type. It is equal to &
			case ET::INT:
				for (const auto& choice : *_choices)
					if (cast_equal<int>(elt,choice))
						return true;
				break;
			case ET::TEXT:
				for (const auto& choice : *_choices)
					if (cast_equal<bst::str>(elt,choice))
						return true;
				break;
			case ET::BOOL:
				for (const auto& choice : *_choices)
					if (cast_equal<bool>(elt,choice))
						return true;
				break;
			case ET::SYMBOL:
				for (const auto& choice : *_choices)
					if (cast_equal<RSymbol>(elt,choice))
						return true;
				break;
			case ET::LIST:
				for (const auto& choice : *_choices)
					if (_list_match(elt,choice,type))
						return true;
				break;
			default:
				assert(false&&"Type not recognized");
		};

		// default return: elt was not found
		return false;
	}

	E::any Parameter::getDefault(const bst::str& lang) {
		/* Return the default parameter if it has one
		 */
		assert((def||_def) && "No default set. Please use Parameter::hasDefault to check it");
		if (!_def) {
			ExprType real_type {getMax(lang) > 1 ? ExprType(ET::LIST,type) : type};
			auto compiler { MonoExprCompiler(*def,real_type,*parent,lang) };
			auto bt {compiler.compile()};
			auto vm { VM(bt,lang,std::vector<E::any>()) };
			_def = std::make_unique<E::any>(vm.run());

			// checks
			if (getMax(lang) > 1) {
				auto vect_def { E::any_cast<std::vector<E::any>>(*_def) };
				if (!between(vect_def.size(),lang))
					throw iexc(exc::ArgumentNBError,"Default arguments doesn't match the number required by the parameter. Default defined here: ",def->getPosition(),"Argument defined here: ", pos);
			}

		}
		return *_def;

	}

	bool Parameter::_list_match(const E::any& first, const E::any& second, const ExprType& type) {
		/* true if first is equal to second.
		 * if type & ET::LIST, call this function
		 */
		switch (type) {
			case ET::INT:
				return cast_equal<int>(first,second);
			case ET::TEXT:
				return cast_equal<bst::str>(first,second);
			case ET::BOOL:
				return cast_equal<bool>(first, second);
			case ET::SYMBOL:
				return cast_equal<RSymbol>(first,second);
			case ET::LIST:
				{
					auto first_c { E::any_cast<std::vector<E::any>>(first) };
					auto second_c { E::any_cast<std::vector<E::any>>(second) };
					// check the size
					if (first_c.size() != second_c.size())
						return false;
					// compare each element
					for (size_t i{0}; i < first_c.size(); ++i) {
						if (!_list_match(first_c[i],second_c[i],type.getContained()))
							return false;
					}
					return true;
				}
				break;
			default:
				assert(false&&"Type unknown");
		};
	}


	// Parameters
	Parameters::Parameters()
	{
	}

	Parameters::Parameters(Symbol& np) :
		parent{&np}
	{
	}

	Parameters& Parameters::operator= (const Parameters& other) {
		/* Assignment
		 */
		for (const auto& elt : other.parms)
			parms.push_back(elt);
		return *this;
	}

	bool Parameters::contains(const bst::str& name) const {
		/* true if name names a parameter
		 */
		for (const auto& p : parms)
			if (p == name)
				return true;
		return false;
	}

	bool Parameters::operator == (const Parameters& others) const {
		/* true if others are exactly equals to *this
		 */

		if (others.parms.size() != parms.size())
			return false;

		for (size_t i{0}; i<others.parms.size();++i)
			if (others.parms[i] != parms[i])
				return false;

		return true;
	}

	PosVect Parameters::getPositions() const {
		/* Return all positions of the parameters
		 */
		PosVect rv;
		for (const auto& prm : parms)
			for (const auto& pos : prm.getPositions())
				rv.push_back(pos);
		return rv;
	}

	void Parameters::push_back(const Parameter& np) {
		/* Add a new parameter
		 */
		// check name collision
		for (const auto& p : parms)
			if (p == np.getName())
				throw iexc(exc::NameError,"Name already taken by another parameter",np.getPositions(),"Name already defined here: ",p.getPositions());

		parms.push_back(np);
		parms.back().setIndex(parms.size()-1);
	}

	bool Parameters::empty() const{
		return parms.empty();
	}

	size_t Parameters::size() const {
		return parms.size();
	}

	const std::vector<Parameter>& Parameters::getList() const{
		return parms;
	}

	void Parameters::setParent(Symbol& np) {
		/*Set parent and the parent
		 * of every Parameter
		 */
		parent = &np;
		for (auto& elt : parms)
			elt.setParent(np);
	}

	std::vector<Parameter>& Parameters::getList(){
		return parms;
	}

	std::vector<Parameter>::iterator Parameters::begin() {
		return parms.begin();
	}

	std::vector<Parameter>::iterator Parameters::end() {
		return parms.end();
	}

	std::vector<E::any> Parameters::formatArgs(const std::vector<Arg>& args, const bst::str& lang) {
		/*Check the arguments and format them
		 */
		std::vector<E::any> formatted {parms.size()};
		auto queue { ParmQueuer(parms,lang) };

		size_t arg_idx{0};
		for (; arg_idx < args.size(); ++arg_idx) {
			// get the argument
			const Arg& arg{args[arg_idx]};
			// get matching parameter
			Parameter& parm{queue(arg)};

			E::any value;
			// check type 
			if (parm.getMax(lang) > 1)
				value = get_multiple_args(args,arg_idx,lang,parm);
			else {
				if (arg.type != parm.getType())
					throw iexc(exc::TypeError,"Argument entered does not match the type of the parameter. Argument: ",arg.pos,"Parameter set here: ",parm.getPositions());
				value = arg.value;
			}

			// check choice 
			if (!parm.choiceMatch(value,lang))
				throw iexc(exc::ValueError,"Value entered does not match the choices set. Value entered: ",arg.pos,"Choices: ",parm.getChoices().getPosition());

			// append to formatted
			formatted.at(static_cast<size_t>(parm.getIndex())) = value;

		}
		// fill default
		if (queue.hasUnfilledDefault()) {
			auto unfilled_def { queue.getUnfilledDefault() };
			for (auto& parm_rf : unfilled_def) {
				auto& parm {parm_rf.get()};
				formatted.at(static_cast<size_t>(parm.getIndex())) = parm.getDefault(lang);
				queue.markFinished(parm);
			}
		}
		// check if every parameter has been checked 
		if (!queue.areParametersFilled()) {
			std::vector<Position> pos;
			for (const auto& arg : args)
				pos.push_back(arg.pos);
			throw iexc(exc::ArgumentNBError,"The number of arguments requireddoes not match the number of arguments entered. Arguments: ", pos,"Parameters defined here: ", getPositions());
		}

		return formatted;
	}

	E::any Parameters::get_multiple_args(const std::vector<Arg>& args, size_t& arg_idx, const bst::str& lang,Parameter& parm) {
		/* Return a multiple arg
		 */
		const bst::str& arg_name { args[arg_idx].name };
		std::vector<E::any> value;

		for (; arg_idx < args.size() && args[arg_idx].name == arg_name; ++arg_idx) {
			const Arg& arg {args[arg_idx]};
			if (args[arg_idx].type != parm.getType())
				throw iexc(exc::TypeError,"Argument entered does not match the type of the parameter. Argument: ",arg.pos,"Parameter set here: ",parm.getPositions());

			value.push_back(arg.value);
		}
		if (!parm.between(lang))
			throw iexc(exc::ArgumentNBError,"Argument number entered does not match the number required. Last argument: ",args[arg_idx-1].pos,"Parameter set here: ",parm.getPositions());
		return value;
	}

	bool operator == (CRParameter& f, CRParameter& s) {
		/* true if f and s points to the same object
		 */
		return &f.get() == &s.get();
	}

}
