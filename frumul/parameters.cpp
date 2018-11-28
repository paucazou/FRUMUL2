#include <climits>
#include <unordered_map>
#include "functions.inl"
#include "parameters.h"
#include "compiler.h"
#include "symbol.h"
#include "vm.h"
#define TEMPASSERT assert(temporary&&"Temporary does not exist")

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

	}

	Parameter::Parameter(const Parameter& other) :
		type{other.type},
		name{other.name},
		def{uniq_copy<Node>(other.def)},
		choices{uniq_copy<Node>(other.choices)},
		pos{other.pos}
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
		 */
		/*
		if (limit1)
			delete limit1;
		if (limit2)
			delete limit2;
			*/
		if (temporary)
			delete temporary;
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

	void Parameter::evaluate() {
		/* evaluate temporary values
		 * default and choices
		 * TODO
		 */

		assert(!temporary&&"Temporary already set");
		temporary = new Temp;
	}

	void Parameter::reset() {
		/* Reset temporary values
		 * dynamically evaluated:
		 * min/max, default and choices
		 */
		delete temporary;
		temporary = nullptr;
	}

	void Parameter::setParent(Symbol& np) {
		/* Set a new parent
		 */
		parent = &np;
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

	const Node& Parameter::getDefault() const {
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

	int Parameter::getMin(const bst::str& lang) const {
		return calculateMinMax(lang).first;
	}

	int Parameter::getMax(const bst::str& lang) const {
		return calculateMinMax(lang).second;
	}

	std::pair<int,int> Parameter::calculateMinMax(const bst::str& lang) const {
		/* Calculate min and max
		 * and returns it, min as first,
		 * and max as second
		 * checks the error (impossible to do this before)
		 */
		assert(parent&&"Parameter: parent is not set");
		if (limit2) { 
			int min { limit1->getLimit(lang,*parent) };
			int max{limit2->getLimit(lang,*parent) };

			// limits are equal
			if (min == max) {
				if (limit1->getComparison() != limit2->getComparison())
					throw iexc(exc::ArgumentNBError,"Limits given to the parameter have the same value but not the same sign. Limit 1:",limit1->getPosition(),"Limit 2:",limit2->getPosition());

				return calculateMinMaxWithOneLimit(min,limit1->getComparison());

			}
			else if (min > max)
				throw iexc(exc::ArgumentNBError,"Min is over max. Min: ",limit1->getPosition(),"Max: ",limit2->getPosition());

			// get real value
			if (limit1->getComparison() == Limit::SUPERIOR)
				++min;
			if (limit2->getComparison() == Limit::INFERIOR)
				--max;

			// last checks
			if (min < 0)
				throw exc(exc::ValueError,"Limit given is under zero",limit1->getPosition());
			return {min,max};

		}

		return calculateMinMaxWithOneLimit(limit1->getLimit(lang,*parent),limit1->getComparison());
	}

	std::pair<int,int> Parameter::calculateMinMaxWithOneLimit(int limit, Limit::Comparison c) const {
		constexpr int absolute_min {0};
		// check errors
		if (limit<0)
			throw exc(exc::ValueError,"Limit given is under zero",limit1->getPosition());

		switch (c) {
			case Limit::EQUAL:
				return {limit,limit};
			case Limit::SUPERIOR:
				return {limit+1,INT_MAX};
			case Limit::INFERIOR:
				return {absolute_min,limit-1};
			case Limit::SEQUAL:
				return {limit,INT_MAX};
			case Limit::IEQUAL:
				return {absolute_min,limit};

		};
		return {-1,-1}; // -Wreturn-type
	}

	const PosVect& Parameter::getPositions() const {
		return pos;
	}

	bool Parameter::operator == (int nb) const {
		/* true if nb is equal to min AND max
		 */
		TEMPASSERT;
		return (nb == temporary->min && nb == temporary->max);
	}

	bool Parameter::operator > (int nb) const {
		/* true if max is above nb
		 */
		TEMPASSERT;
		return temporary->max > nb;
	}

	bool Parameter::operator < (int nb) const {
		/* true if min is beyond nb
		 */
		return temporary->min < nb;
	}

	bool Parameter::between (int nb) const {
		/* true if nb is between or equal to min
		 * and max
		 */
		return (nb <= temporary->max && nb >= temporary->min);
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
		}
		return i;
	}

	Parameter::Limit::Comparison Parameter::Limit::getComparison() const {
		return comparison;
	}

	const Position& Parameter::Limit::getPosition() const {
		return pos;
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
	}

	bool Parameters::empty() const{
		return parms.empty();
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
		std::vector<E::any> formatted;
		// maps that saves the number of times a parameter
		// has been called
		std::unordered_map<CRParameter,unsigned int> call_number;
		const auto defaultSetMap { &defaultSet<std::unordered_map<CRParameter,unsigned int>,CRParameter,unsigned int> };

		size_t arg_idx{0};
		for (; arg_idx < args.size(); ++arg_idx) {
			// get the argument
			const Arg& arg{args[arg_idx]};
			// get matching parameter
			const Parameter& parm{parms[arg_idx]};
			CRParameter crparm{parm};
			// check type TODO vérifier si un argument est seulemnet un membre de liste
			if (arg.type != parm.getType())
				throw iexc(exc::TypeError,"Argument entered does not match the type of the parameter. Argument: ",arg.pos,"Parameter set here: ",parm.getPositions());
			// check number
			unsigned int call_nb { defaultSetMap(call_number,crparm,0) };
			if (call_nb > static_cast<unsigned int>(parm.getMax(lang)))
				throw iexc(exc::ArgumentNBError,"Too many arguments entered for the required parameter",arg.pos,"Parameter defined here: ",parm.getPositions());
			// check choice TODO
			// append to formatted
			formatted.push_back(arg.value);

		}
		// check if every parameter has been checked TODO or args match the minimum required
		return formatted;
	}

	bool operator == (CRParameter& f, CRParameter& s) {
		/* true if f and s points to the same object
		 */
		return &f.get() == & s.get();
	}

}
