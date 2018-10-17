#include <cassert>
#include "schildren.h"

namespace frumul {

	Schildren::Schildren () :
		parent{nullptr}
	{
	}

	Schildren::Schildren (Symbol& np) :
		parent{&np}
	{
	}

	Symbol& Schildren::getChild(const Node& node) {
		/* getter-setter: if no child is found,
		 * return a new one with the name
		 */
		Symbol* s{nullptr};
		if (hasChild(node)) {
			if (node.type() == Node::LINKED_NAMES)
				for (auto& child : children) {
					if (child.getName() == node.get("short").getValue() || child.getName() == node.get("long").getValue()) {
						s = &child;
						break;
					}
				}
			else // node Short or Long
				for (auto& child : children) {
					if (child.getName() == node.getValue()) {
						s = &child;
						break;
					}
			}
		}
		else  // create new child
			s = &appendChild();
		// update name
		s->getName().add(node);
		return *s;
	}

	Symbol& Schildren::getChild(const bst::str& name) {
		/* name can be a long or a short name
		 */
		for (auto& child : children)
			if (child.getName() == name)
				return child;
		assert(false&&"child does not exist");
	}

	bool Schildren::hasChildren () const {
		/* true if it has children
		 */
		return !children.empty();
	}

	Schildren::operator bool () const {
		/* true if it has children
		 * overloaded function
		 */
		return hasChildren();
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

	// finders
	
	Symbol& Schildren::find(const bst::str& path, PathFlag flag) {
		/* Looks in children to find a requested symbol.
		 * Order : long name, short name, separation dots.
		 * If it has a remain, transmits it to the child
		 * found. No privileged parameter allowed.
		 */
		assert(path&&"path is empty");
		// relative path allowed ?
		if (flag & PathFlag::Relative) {
			if (path.uAt(0) == "§") {
				if (!(*parent).hasParent()) // error: no parent
					throw path;
				bst::str npath {path.uRange(1,path.uLength()-1)};
				if (npath)
					return (*parent).getParent().getChildren().find(npath,flag);
				return (*parent).getParent();
			}

			// remove \ if necessary
			if (path.uAt(0) == "\\")
				// BUG if a symbol named '§' follows another named '\', as in : \\§. Corner case
				if (path.uLength() >= 2 && (path.uAt(1) == "§" || path.uAt(1) == "\\")) 
					return find(path.uRange(1,path.uLength()-1),flag);
		}
		// try to find a long name
		for (auto& child : children) {
			const bst::str& name {child.getName().getLong()};
			
			if (!name || name.uLength() > path.uLength())
				continue;

			if (path.uRange(0,name.uLength() -1) == name) {

				if (name.uLength() == path.uLength())
					return child;
				bst::str npath{path.uRange(name.uLength(),path.uLength() -1)};
				return child.getChildren().find(npath);
				
			}
		}
		// try to find a short name. Yes, lot of duplicates
		for (auto& child : children) {
			const bst::str& name {child.getName().getShort()};

			if (!name)
				continue;

			if (path.uAt(0) == name) {
				if (path.uLength() == 1)
					return child;

				bst::str npath{path.uRange(1,path.uLength() - 1) };
				return child.getChildren().find(npath);
			}
		}

		// is there a dot ?
		if (path.uAt(0) == ".")
			if (path.uLength() > 1)
				return find(path.uRange(1,path.uLength() -1));

		throw path; // this must be caught
	}


	// const getters

	const Symbol& Schildren::getChild(const bst::str& name) const {
		/* get Child in constant context
		 */
		for (const auto& child : children)
			if (child.getName() == name)
				return child;
		assert(false&&"No child of this name");
	}

	const std::list<Symbol>& Schildren::getChildren() const {
		/* get the children
		 */
		return children;
	}
	

	Symbol& Schildren::addChild(const Symbol& s) {
		/* Append s to children
		 * and return a reference to its copy
		 */
		children.push_back(s);
		return children.back();
	}

	Symbol& Schildren::appendChild() {
		/* creates a new empty child
		 * and return a reference to it
		 */
		children.emplace_back();
		return children.back();
	}

	Symbol& Schildren::appendChild(const bst::str& name) {
		/* Creates a new child with name
		 * This should not be used by the regular
		 * interpreter engine since it cannot trace
		 * the position of the name.
		 */
		Symbol& s {appendChild()};
		s.getName().add(name);
		return s;
	}
}
