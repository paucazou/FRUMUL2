#include "symbol.h"

namespace frumul {
	// alias
	Alias::Alias()
	{
	}

	Alias::Alias(const Alias& other) :
		val{other.val}, path{other.path}
	{
		assert(!val&&"Doesn't work, because the pointer points to garbage...");
		if (other.pos)
			pos = std::make_unique<Position>(*other.pos);
	}

	Alias::operator bool () const {
		/* true if it is a valid and usable
		 * alias
		 */
		return val;
	}

	bool Alias::hasPath() const {
		/* true if a path has already been set
		 */
		return path;
	}

	const Position& Alias::getPosition() const {
		/* return the position of the alias
		 */
		return *pos;
	}

	const Symbol& Alias::getVal() const {
		/* Unsafe function
		 * that get the symbol to which points
		 * the alias.
		 */
		assert(val&&"Pointer is not set");
		return *val;
	}

	const FString& Alias::getPath() const {
		/* return path
		 * value
		 */
		return path;
	}

	void Alias::setPath(const Node& node) {
		/* save the path which points
		 * to the symbol.
		 * Sets the position with it.
		 */
		assert(node.type() == Node::ALIAS_VALUE&&"node has not required type: ALIAS_VALUE");
		if (path)
			throw iexc(exc::AliasAlreadySet,"You can't set two alias to one symbol.",node.getPosition(),"Alias already set here:",*pos);

		path = node.getValue(); 
		setPosition(node.getPosition());
	}

	void Alias::setVal(const Symbol& nalias) {
		/* set the pointer to symbol
		 * Please check before that the pointer
		 * has been set yet.
		 */
		assert(!val&&"Alias pointer already set");
		val = &nalias;
	}

	void Alias::setPosition(const Position& npos) {
		/*Set the pointer to position
		 */
		assert(!pos&&"Position of the alias has already been set");
		pos = std::make_unique<Position>(npos);
	}

	// Symbol
	
	Symbol::Symbol()
	{
	}

	Symbol::Symbol(const Symbol& other) :
		name{other.name}, mark{other.mark}, children{std::make_unique<Schildren>(*other.children)}, parent{other.parent}, alias{other.alias}, 
		value{std::make_unique<Value>(*other.value)}
	{
	}

	// getters

	Name& Symbol::getName() {
		/* Return name
		 */
		return name;
	}

	Mark& Symbol::getMark() {
		/* Return mark
		 */
		return mark;
	}

	Alias& Symbol::getAlias() {
		/* return alias
		 */
		return alias;
	}

	Schildren& Symbol::getChildren() {
		/* return children
		 */
		return *children;
	}

	Symbol& Symbol::getParent() {
		/* return parent
		 */
		return *parent;
	}

	Value& Symbol::getValue() {
		/* Return value
		 */
		return *value;
	}

	Parameters& Symbol::getParameters() {
		/* Return parameters
		 */
		return parameters;
	}

	std::vector<FString> Symbol::getChildrenNames() {
		/* Return a vector of names used by the immediate
		 * children of *this
		 */
		std::vector<FString> names;
		for (const auto& child : children->getChildren()) {
			const Name& child_n{child.getName()};
			if (child_n.hasLong())
				names.push_back(child_n.getLong());
			if (child_n.hasShort())
				names.push_back(child_n.getShort());
		}
		return names;
	}

	// const getters

	const ExprType& Symbol::getReturnType() const {
		/* Get the return type of the value
		 */
		return return_type.type;
	}

	const Position& Symbol::getReturnTypePos() const {
		return *return_type.pos;
	}
	
	const Name& Symbol::getName() const {
		return name;
	}

	const Alias& Symbol::getAlias() const {
		return alias;
	}

	const Schildren& Symbol::getChildren() const {
		return *children;
	}

	const Symbol& Symbol::getParent() const {
		assert(parent&&"No parent has been set");
		return *parent;
	}

	const Value& Symbol::getValue() const {
		return *value;
	}

	const Parameters& Symbol::getParameters() const {
		return parameters;
	}

	// setters
	
	void Symbol::setParent(Symbol& nparent) {
		/* Set parent
		 */
		if (!parent)
			parent = &nparent;
		assert(parent == &nparent&&"Parent doesn't match");
	}

	void Symbol::setParameters(const Parameters& parms) {
		/* Set parameters
		 */
		assert(parameters.empty()&&"Parameters are not empty");
		parameters = parms;
		parameters.setParent(*this);
	}

	void Symbol::setReturnType(const Node& node) { 
		/* Set the return type.
		 */
		assert(node.type() == Node::RETURN_TYPE&&"Node type should be RETURN_TYPE");
		return_type.type = ExprType(node);
		return_type.pos = std::make_unique<Position>(node.getPosition());

		if (return_type.type.isConst() || return_type.type.isStatic())
			throw exc(exc::TypeError,"Symbol return type can not be const or static",node.getPosition());
		
	}

	void Symbol::addUnsafeArgsToParms() {
		/* The unsafe arguments will be add to the parameters.
		 * This function works only once.
		 */

		if (mark_added_to_parameters)
			return;

		auto& pos { mark.getPositions() };
		for (int i{0}; i < mark.afterArgsNumber(); ++i) {
			parameters.push_back(Parameter(unsafe_name + i, ET::TEXT,pos,*this));
		}
		mark_added_to_parameters = true;
	}
	
	// booleans
	bool Symbol::hasParent() const {
		/* true if parent exists
		 */
		return parent;
	}

	bool Symbol::hasAlias() const {
		/* true if instance
		 * is an alias
		 */
		 return alias;
	}

	bool Symbol::hasMark() const {
		/* true if mark has been set
		 */
		return mark.get();
	}

	bool Symbol::hasChildren() const {
		/* true if it has children
		 */
		return children->hasChildren();
	}

	bool Symbol::hasValue() const {
		/* true if symbol has a value
		 */
		return *value;
	}

	// use
	FString Symbol::call(const std::vector<std::any>& args, const FString& lang) {
		/* Call the symbol
		 * The args must be formatted first
		 */
		// checks
		if (return_type.type != ExprType::TEXT)
			throw BackException(exc::TypeError);
		checkCall(lang);

		// execution
		std::any r{value->execute(lang,args)};
		return std::any_cast<FString>(r);
	}
	
	std::any Symbol::any_call(const std::vector<Arg>& args, const FString& lang) {
		/* Calls the symbol
		 * and return an any value
		 * Return type check should have be done before
		 */
		std::vector<std::any> formatted_args;
		// checks
		checkCall(lang);
		if (args.size() == 0 && !parameters.empty())
			throw BackException(exc::ArgumentNBError);
		else if (args.size() > 0) {
			formatted_args = parameters.formatArgs(args,lang);
		}

		return value->execute(lang,formatted_args);
	}

	void Symbol::checkCall(const FString& lang) {
		/* Does the checks for a call
		 * Does not the return type check
		 */
		// check language
		if (!value->canExecuteWith(lang))
			throw BackException(exc::LangError);
	}


	// display
	FString Symbol::toString() const {
		/* String representation of
		 * a symbol
		 */
		FString s{"<Symbol>\n"};
		// name
		s += name.names() + '\n';

		// mark
		s += "Number of tags expected: " + FString(mark.get()) + '\n';

		// return type
		if (return_type.pos) {
			s += "Return type: ";
			s += return_type.type.toString();
			s += '\n';
		}

		// parent
		s += "Parent: " + FString(parent ? "Yes":"No") + '\n';

		// alias
		if (alias.hasPath())
			s += "Points to: " + alias.getPath() + '\n';

		// langs/values available
		if (value) {
			s += "Languages available: ";
			for (const auto& l : value->getLangs())
				s += l.getName() + ". ";
			s += '\n';
		}
		// children
		if (children) {
			s += "Children:\n";
			for (const auto & child : children->getChildren())
				s += child.getName().names() + ". ";
			s += '\n';
		}

		return s;
	}
}




		


