#include <cassert>
#include "node.h"

namespace frumul {
	Node::Node (const Node::Type ntype, const Position& npos, const std::map<bst::str,Node>& nchildren) :
		node_type{ntype}, pos{npos}, children{nchildren}
	{}

	Node::Node (const Node& n) :
		node_type{n.node_type}, pos{n.pos}, children{n.children}
	{}

	const Position& Node::getPosition() const {
		/* Return the position of the node
		 * in the text
		 */
		return pos;
	}

	const Node& Node::get (const bst::str& key) const {
		/* Read only function to get
		 * the node matching with t
		 */
		return children.at(key);
	}

	void Node::addChild(const bst::str& name, const Node& child) {
		/* Add a child in children.
		 */
		children.insert(std::pair<bst::str,Node>(name,child));
	}

	Node::Type Node::type() const {
		return node_type;
	}

	const bst::str Node::toString() const {
		/* Return a string
		 * presenting the object
		 */
		bst::str s{"<NODE|" + typeToString(node_type) + ">\n"};
		for (const auto& pair : children) 
			s += pair.first + ": " + typeToString(pair.second.node_type) + "\n";
		s += pos.toString();
		return s;
	}

	bst::str Node::typeToString(Node::Type t) {
		/* Return a readable value
		 * for type
		 */
		bst::str s;
		switch (t) {
			case Node::DOCUMENT:
				s = "DOCUMENT";
				break;
			case Node::EMPTY:
				s = "EMPTY";
				break;
			case Node::HEADER:
				s = "HEADER";
				break;
			case Node::STATEMENT_LIST:
				s = "STATEMENT_LIST";
				break;
			case Node::TEXT:
				s = "TEXT";
				break;
			case Node::SIMPLE_TEXT:
				s= "SIMPLE_TEXT";
				break;
			default:
				assert(false&&"Unrecognized node type");
		};

		return s;
	}

	std::ostream& operator<< (std::ostream& out, const Node& n) {
		/* Prints the node on stdout
		 */
		out << n.toString();
		return out;
	}

} // namespace
