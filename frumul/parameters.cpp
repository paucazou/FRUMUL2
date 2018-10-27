#include "parameters.h"
#define TEMPASSERT assert(temporary&&"Temporary does not exist")

namespace frumul {

	Parameter::Parameter(const Node& node) :
		type{node.get("variable").get("type")},
		name{node.get("variable").get("name").getValue()}
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
			limit1 = new Limit(*other.limit1);
		if (other.limit2)
			limit2 = new Limit(*other.limit2);

	}

	Parameter::~Parameter () {
		/* Destructor.
		 */
		if (limit1)
			delete limit1;
		if (limit2)
			delete limit2;
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
			Comparison c{comparisonValue(arg.getValue())};
			limit1 = new Limit{
					arg.getNumberedChildren()[0],
					c};
			if (fields.count("argnb1")) {
				const Node& arg {fields.at("argnb1")};
				Comparison c{comparisonValue(arg.getValue())};
				limit2 = new Limit(
					arg.getNumberedChildren()[0],
					c);
			}
		}
		else {
			// implicit: =1
			limit1 = new Limit(1,EQUAL);
		}


	}

	void Parameter::evaluate() {
		/* evaluate temporary values
		 * min/max, default and choices
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

	Parameter::Comparison Parameter::comparisonValue(const bst::str& val) const{
		/* Return an enum
		 * matching with val
		 */
		if (val == "=")
			return EQUAL;
		else if (val == ">")
			return SUPERIOR;
		else if (val == "<")
			return INFERIOR;
		else if (val == ">=")
			return SEQUAL;
		else if (val == "<=")
			return IEQUAL;
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

	int Parameter::getMin() const {
		TEMPASSERT;
		return temporary->min;
	}

	int Parameter::getMax() const {
		TEMPASSERT;
		return temporary->max;
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

	Parameter::Limit::Limit(const Node& n, Comparison c) :
		comparison{c}, isNode{true}, node{ new Node(n)}
	{
		/* Constructs with a node
		 * yet to be evaluated
		 */
	}

	Parameter::Limit::Limit (int ni, Comparison c) :
		comparison{c}, isNode{false}, i{ni}
	{
		/* Constructs directly an int
		 */
	}

	Parameter::Limit::~Limit() {
		/*Destructor
		 * TODO is int deleted ?
		 */
		if (isNode)
			delete node;
	}

	int Parameter::Limit::getLimit() const {
		/* Return the limit as an int.
		 * Evaluates the node (if there is a node)
		 * or return the int
		 * TODO
		 */
		if (isNode)
			assert(false&&"Node evaluation not yet ready");
		return i;
	}

	bool Parameter::Limit::isConform(int x) const {
		/* true if x respects the limit
		 */
		int limit {getLimit()};
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
				assert(false&&"Comparison operator not correctly set");
		};
		return false;
	}

	// Parameters
	Parameters::Parameters()
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

	std::vector<Parameter>& Parameters::getList(){
		return parms;
	}

}
