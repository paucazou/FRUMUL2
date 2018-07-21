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

	Symbol& Schildren::getChild(const Node& node) {
		/* getter-setter: if no child is found,
		 * return a new one with the name
		 */
		if (hasChild(node)) {
			if (node.type() == Node::LINKED_NAMES)
				for (const auto& child : children) {
					if (child.getName() == node.get("short").getValue() || child.getName() == node.get("long").getValue()) {
						Symbol& s{child};
						break;
					}
				}
			else // node Short or Long
				for (const auto& child : children) {
					if (child.getName() == node.getValue()) {
						Symbol& s{child};
						break;
					}
			}
		else  // create new child
			Symbol& s{appendChild()};
		// update name
		s.getName().add(node);
		return s;
	}

	Symbol& getChild(const bst::str& name) const {
		/* name can be a long or a short name
		 */
		for (const auto& child : children)
			if (child.getName() == name)
				return child;
		assert(false&&"child does not exist");
	}

	bool Schildren::hasChildren () const {
		/* true if it has children
		 */
		return !children.empty();
	}

	bool Schildren::hasChild (const bst::str& name) const {
		/* true if child of name exists
		 */
		for (const auto& child : children)
			if (child.getName() == name)
				return true;
		return false;
	}

	bool Schildren::hasChild(const Node& node) const {
		/* true if node contains a name
		 * contained by one child.
		 */
		switch (node.type()) {
			case Node::LINKED_NAMES:
				return hasChild(node.get("short")) || hasChild(node.get("long"));
			case Node::SHORT_NAME:
			case Node::LONG_NAME:
				return hasChild(node.getValue());
			default:
				assert(false&&"Node does not contain names");
		}
		return false; // Warning, etc.
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

	Symbol& appendChild(const bst::str& name) {
		/* Creates a new child with name
		 * This should not be used by the regular
		 * interpreter engine since it cannot trace
		 * the position of the name.
		 */
		Symbol& s {appendChild()};
		s.getName().addName(name);
		return s;
	}

