#include "symbol.h"

namespace frumul {
	// alias
	Alias::Alias()
	{
	}

	Alias::operator bool () const {
		/* true if it is a valid and usable
		 * alias
		 */
		return val;
	}

	const Position& Alias::getPosition() const {
		/* return the position of the alias
		 */
		return pos;
	}

	const Symbol& Alias::getVal() const {
		/* Unsafe function
		 * that get the symbol to which points
		 * the alias.
		 */
		assert(val&&"Pointer is not set");
		return *val;
	}

	void Alias::setPath(const Node& node) {
		/* save the path which points
		 * to the symbol.
		 * TODO not finished
		 */
		assert(false&&"Path not yet set in alias");
		path = node.getValue(); // probably not good
	}

	void Alias::setVal(const Symbol& nalias) {
		/* set the pointer
		 * Please check before that the pointer
		 * has been set yet.
		 */
		assert(!val&&"Alias pointer already set");
		val = &nalias;
	}

	// Symbol
	
	Symbol::Symbol()
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




		


