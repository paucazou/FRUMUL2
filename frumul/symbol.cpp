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

	const bst::str& Alias::getPath() const {
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
		name{other.name}, mark{other.mark}, children{other.children}, parent{other.parent}, alias{other.alias}, 
		value{other.value}
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
		return children;
	}

	Value& Symbol::getValue() {
		/* Return value
		 */
		return value;
	}

	Parameters& Symbol::getParameters() {
		/* Return parameters
		 */
		return parameters;
	}

	// const getters
	
	const Name& Symbol::getName() const {
		return name;
	}

	const Alias& Symbol::getAlias() const {
		return alias;
	}

	const Schildren& Symbol::getChildren() const {
		return children;
	}

	const Symbol& Symbol::getParent() const {
		assert(parent&&"No parent has been set");
		return *parent;
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
		return children.hasChildren();
	}

	bool Symbol::hasValue() const {
		/* true if symbol has a value
		 */
		return value;
	}

	// display
	bst::str Symbol::toString() const {
		/* String representation of
		 * a symbol
		 */
		bst::str s{"<Symbol>\n"};
		// name
		s += name.names() + '\n';

		// mark
		s += "Number of tags expected: " + bst::str(mark.get()) + '\n';

		// parent
		s += "Parent: " + bst::str(parent ? "Yes":"No") + '\n';

		// alias
		if (alias.hasPath())
			s += "Points to: " + alias.getPath() + '\n';

		// langs/values available
		if (value) {
			s += "Languages available: ";
			for (const auto& l : value.getLangs())
				s += l.getName() + ". ";
			s += '\n';
		}
		// children
		if (children) {
			s += "Children:\n";
			for (const auto & child : children.getChildren())
				s += child.getName().names() + ". ";
			s += '\n';
		}

		return s;
	}
}




		


