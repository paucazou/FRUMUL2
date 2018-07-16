#include "schildren.h"

namespace frumul {

	Schildren::Schildren () :
		parent{nullptr}
	{
	}

	Schildren::Schildren (const Symbol& np) :
		parent{&np}
	{
	}

	Symbol& getChild(const bst::str& name) {
		/* name can be a long or a short name
		 * If no symbol of this name is found,
		 * it creates one
		 */
		for (const auto& child : children)
			if (name == child.getName())
				return child;
		return appendChild();
	}

	bool Schildren::hasChildren () const {
		/* true if it has children
		 */
		return !children.empty();
	}

	bool Schildren::hasParent () const {
		/* true if the instance
		 * has a parent
		 */
		return parent;
	}
	

	Symbol& Schildren::addChild(const Symbol& s) {
		/* Append s to children
		 * and return a reference to its copy
		 */
		children.push_back(s);
		return children.back();
	}
	Symbol& appendChild() {
		/* creates a new empty child
		 * and return a reference to it
		 */
		Symbol s;
		children.push_back(s);
		return children.back();
	}
