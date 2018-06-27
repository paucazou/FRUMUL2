#include <cassert>
#include "node.h"

namespace frumul {
	Node::Node (const Node::Type ntype, const Position& npos, const std::map<bst::str,Node>& nchildren, const bst::str& nvalue) :
		node_type{ntype}, pos{npos}, numbered_children{nchildren}, value{nvalue}, childrenNamed{true}
	{}

	Node::Node (const Node::Type ntype, const Position& npos, const std::vector<Node>& nchildren, const bst::str& nvalue) :
		node_type{ntype}, pos{npos}, numbered_children{nchildren}, value{nvalue}, childrenNamed{false}
	{}

	Node::Node (const Node& n) :
		node_type{n.node_type}, pos{n.pos}, children{n.children}
	{}

	Node::Type Node::type() const {
		return node_type;
	}

	bool areChildrenNamed () const {
		/* true if children are a map,
		 * false if a vector
		 */
		return childrenNamed;
	}

	const Position& Node::getPosition() const {
		/* Return the position of the node
		 * in the text
		 */
		return pos;
	}

	const Node& Node::get (const bst::str& key) const {
		/* Read only function to get
		 * the node matching with key 
		 */
		assert(childrenNamed&&"Children are a vector.");
		return named_children.at(key);
	}

	const Node& Node::get (int index) const {
		/* Overloaded function.
		 * Return node matching with
		 * index.
		 */
		assert(!childrenNamed&&"Children are a map.");
		return numbered_children.at(index);
	}

	void Node::addChild(const bst::str& name, const Node& child) {
		/* Add a child in children.
		 */
		assert(childrenNamed&&"Children are a vector.");
		named_children.insert(std::pair<bst::str,Node>(name,child));
	}

	void Node::addChild(const Node& child) {
		/* Overloaded function.
		 * Add a child in children
		 */
		assert(childrenNamed&&"Children are a map.");
		numbered_children.push_back(child);
	}


	const bst::str Node::toString() const {
		/* Return a string
		 * presenting the object
		 */
		bst::str s{"<NODE|" + typeToString(node_type) + ">\n"};

		if (childrenNamed) {
			for (const auto& pair : children) 
				s += pair.first + ": " + typeToString(pair.second.node_type) + "\n";
		} else {
			for (int i{0}; i < numbered_children.length(); ++i)
				s += bst::str(i) + ": " + typeToString(numbered_children.at(i).node_type) + "\n";
		}

		s += pos.toString();
		return s;
	}

	
	std::ostream& operator<< (std::ostream& out, const Node& n) {
		/* Prints the node on stdout
		 */
		out << n.toString();
		return out;
	}

} // namespace
