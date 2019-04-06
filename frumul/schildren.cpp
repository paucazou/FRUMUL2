#include <algorithm>
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
				for (auto child : children) {
					if (child->getName() == node.get("short").getValue() || child->getName() == node.get("long").getValue()) {
						s = child;
						break;
					}
				}
			else // node Short or Long
				for (auto child : children) {
					if (child->getName() == node.getValue()) {
						s = child;
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

	Symbol& Schildren::getChild(const FString& name) {
		/* name can be a long or a short name
		 */
		for (auto child : children)
			if (child->getName() == name)
				return *child;
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

	bool Schildren::hasChild (const FString& name) const {
		/* true if child of name exists
		 */
		for (const auto child : children)
			if (child->getName() == name)
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

	std::vector<Symbol*> Schildren::sortChildrenByLongName() {
		/* Return a vector of pointers 
		 * of the children sorted by long names, the longest first
		 */
		std::vector<Symbol*> sorted;
		for (auto child : children)
			sorted.push_back(child);

		std::sort(sorted.begin(), sorted.end(), [](Symbol* a, Symbol* b) {
				return a->getName().getLong().length() > b->getName().getLong().length();
				});

		return sorted;
	}

	// finders
	
	TailResult Schildren::find(const FString& path, const PathFlag flag) {
		/* Looks in children to find a requested symbol.
		 * Order : long name, short name, separation dots.
		 * If it has a remain, transmits it to the child
		 * found. No privileged parameter allowed.
		 */
		assert(path&&"path is empty");
		// relative path allowed ?
		if (flag & PathFlag::Relative) {
			if (path[0] == "§") {
				if (!(*parent).hasParent()) // error: no parent
					throw path;
				FString npath {path.extract(1,path.length()-1)};
				if (npath)
					return (*parent).getParent().getChildren().find(npath,flag);
				return (*parent).getParent();
			}

			// remove \ if necessary
			if (path[0] == "\\")
				// BUG if a symbol named '§' follows another named '\', as in : \\§. Corner case
				if (path.length() >= 2 && (path[1] == "§" || path[1] == "\\")) 
					return find(path.extract(1,path.length()-1),flag);
		}
		// try to find a long name
		for (auto child_ptr : sortChildrenByLongName()) {
			auto& child {*child_ptr};
			const FString& name {child.getName().getLong()};
			
			if (!name || name.length() > path.length())
				continue;

			if (path.extract(0,name.length() -1) == name) {

				if (name.length() == path.length())
					return child;
				FString npath{path.extract(name.length(),path.length() -1)};
				return child.getChildren().find(npath,flag);
				
			}
		}
		// try to find a short name. Yes, lot of duplicates
		for (auto child_ptr : children) {
			auto& child = *child_ptr;
			const FString& name {child.getName().getShort()};

			if (!name)
				continue;

			if (path[0] == name) {
				if (path.length() == 1)
					return child;

				FString npath{path.extract(1,path.length() - 1) };
				return child.getChildren().find(npath,flag);
			}
		}

		// is there a dot ?
		if (path[0] == ".")
			if (path.length() > 1) {
				try {
					return find(path.extract(1,path.length() -1),flag);
				} catch (const FString&) {
					return _findRestOfTail(path,flag);
				}
			}
		if (flag & PathFlag::Privileged || flag & PathFlag::Parameter)
			return _findRestOfTail(path,flag);

		throw path; // this must be caught
	}


	// const getters

	const Symbol& Schildren::getChild(const FString& name) const {
		/* get Child in constant context
		 */
		for (const auto child : children)
			if (child->getName() == name)
				return *child;
		assert(false&&"No child of this name");
	}

	const std::vector<Symbol*>& Schildren::getChildren() const {
		/* get the children
		 */
		return children;
	}
	

	Symbol& Schildren::addChild(const Symbol& s) {
		/* Append s to children
		 * and return a reference to its copy
		 */
		owned_children.push_back(s);
		children.push_back(&owned_children.back());
		return owned_children.back();
	}

	Symbol& Schildren::addChildReference(Symbol& s) {
		/* Does not copy s, but
		 * add a reference to it
		 * Does not take the ownership
		 */
		children.push_back(&s);
		return s;
	}

	Symbol& Schildren::appendChild() {
		/* creates a new empty child
		 * and return a reference to it
		 */
		owned_children.emplace_back();
		children.push_back(&owned_children.back());
		return owned_children.back();
	}

	Symbol& Schildren::appendChild(const FString& name) {
		/* Creates a new child with name
		 * This should not be used by the regular
		 * interpreter engine since it cannot trace
		 * the position of the name.
		 */
		Symbol& s {appendChild()};
		s.getName().add(name);
		return s;
	}

	TailResult Schildren::_findRestOfTail(const FString& path, const PathFlag flag) {
		/* Manages the end of the tail and return the correct result
		 */
		auto result { TailResult(*parent) };

		if ((flag & PathFlag::Parameter) && parent->getParameters().contains(path)) {
			result.setParameterName(path);
		} else if (flag & PathFlag::Privileged) {
			result.setPrivilegedArgument(path);
		}
		else
			throw path;
		return result;
	}
}
