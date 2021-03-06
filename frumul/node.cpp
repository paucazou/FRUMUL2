#include <cassert>
#include "node.h"

#define ASSERT_CHILDREN_NAMED \
	assert(childrenNamed&&"Children are not named")
#define ASSERT_CHILDREN_NUMBERED \
	assert(!childrenNamed&&"Children are not numbered")

namespace frumul {
	Node::Node (const Node::Type ntype, const Position& npos, const std::map<FString,Node>& nchildren, const FString& nvalue) :
		node_type{ntype}, pos{npos}, named_children{nchildren}, value{nvalue}, childrenNamed{true}
	{
		/* Constructs the named_children
		 * object with a value
		 */
	}

	Node::Node (const Node::Type ntype, const Position& npos, const std::vector<Node>& nchildren, const FString& nvalue):
		node_type{ntype}, pos{npos}, numbered_children{nchildren}, value{nvalue}, childrenNamed{false}
	{
		/* Constructs the numbered_children
		 * object with a value
		 */
	}

	Node::Node (const Type ntype, const Position& npos, const FString& nvalue) :
		node_type{ntype}, pos{npos}, numbered_children{}, value{nvalue}, childrenNamed{false}
	{
		/* No child, only value
		 */
	}

	Node::Node (const Type ntype, const Position& npos, const std::map<FString,Node>& nchildren) :
		node_type{ntype}, pos{npos}, named_children{nchildren}, childrenNamed{true}
	{
		/* Constructs the named_children
		 * and no value
		 */
	}

	Node::Node (const Type ntype, const Position& npos, const std::vector<Node>& nchildren) :
		node_type{ntype}, pos{npos}, numbered_children{nchildren}, childrenNamed{false}
	{
		/* Constructs the numbered_children
		 * and no value
		 */
	}

	Node::Node (const Node& n) :
		node_type{n.node_type}, pos{n.pos}, 
		value{n.value}, childrenNamed{n.childrenNamed}
	{
		/* copy constructor
		 */
		if (childrenNamed) {
			new (&named_children) std::map<FString,Node>;
			named_children = n.named_children;
		}
		else {
			new (&numbered_children) std::vector<Node>;
			numbered_children = n.numbered_children;
		}
	}

	Node::Type Node::type() const {
		return node_type;
	}

	Node::~Node () {
		if (childrenNamed)
			named_children.~map<FString,Node>();
		else
			numbered_children.~vector<Node>();
	}

	bool Node::areChildrenNamed () const {
		/* true if children are a map,
		 * false if a vector
		 */
		return childrenNamed;
	}

	bool Node::has(const FString& key) const {
		/* true if node has 'key' in his 
		 * children
		 * false if children are not named
		 */
		if (!childrenNamed)
			return false;
		return named_children.count(key) > 0;
	}

	bool Node::has(int index) const {
		/* true if node has index
		 * in his children
		 * false if children are named
		 */
		if (childrenNamed || index < 0)
			return false;
		return static_cast<unsigned int>(index) < numbered_children.size();
	}

	const Position& Node::getPosition() const {
		/* Return the position of the node
		 * in the text
		 */
		return pos;
	}

	const FString& Node::getValue () const {
		/* Return the value
		 * saved in the node
		 * It can be an empty one
		 */
		return value;
	}

	const Node& Node::get (const FString& key) const {
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
		return numbered_children.at(static_cast<size_t>(index));
	}

	const std::map<FString,Node>& Node::getNamedChildren () const {
		/* Return a reference to 
		 * the named children
		 * if they exist.
		 */
		assert(childrenNamed&&"Children are not named");
		return named_children;
	}

	const std::vector<Node>& Node::getNumberedChildren () const {
		/* Return a reference to the numbered
		 * children if they 
		 * exist
		 */
		assert(!childrenNamed&&"Children are not numbered");
		return numbered_children;
	}

	unsigned int Node::size() const {
		/* Return the number of children
		 */
		if (childrenNamed)
			return named_children.size();
		else
			return numbered_children.size();
	}

	std::map<FString,Node>::reverse_iterator Node::rbegin(bool named) {
		ASSERT_CHILDREN_NAMED;
		assert(named&&"not named");
		return named_children.rbegin();
	}

	std::vector<Node>::const_reverse_iterator Node::rbegin() const {
		ASSERT_CHILDREN_NUMBERED;
		return numbered_children.rbegin();
	}

	std::map<FString,Node>::reverse_iterator Node::rend(bool named) {
		ASSERT_CHILDREN_NAMED;
		assert(named&&"not named");
		return named_children.rend();
	}

	std::vector<Node>::const_reverse_iterator Node::rend() const {
		ASSERT_CHILDREN_NUMBERED;
		return numbered_children.rend();
	}

	void Node::addChild(const FString& name, const Node& child) {
		/* Add a child in children.
		 */
		assert(childrenNamed&&"Children are a vector.");
		named_children.insert(std::pair<FString,Node>(name,child));
	}

	void Node::addChild(const Node& child) {
		/* Overloaded function.
		 * Add a child in children
		 */
		assert(!childrenNamed&&"Children are a map.");
		numbered_children.push_back(child);
	}

	void Node::removeChild(const FString& name) {
		/* Remove a child if the children
		 * are a map
		 */
		assert(childrenNamed&&"Children are a vector");
		named_children.erase(name);
	}

	void Node::removeChild(const int i) {
		/* Remove a child if the children
		 * are a vector
		 */
		assert(!childrenNamed&&"Children are a map");
		numbered_children.erase(numbered_children.begin() + (i-1));
	}

	void Node::operator= (const Node& n) {
		/* Copy non constant members of n inside
		 * this
		 * Should not be used, actually
		 */
		assert(false&&"Operator= should not be used whith Node.");
		if (n.childrenNamed)
			named_children = n.named_children;
		else
			numbered_children = n.numbered_children;
	}


	const FString Node::toString() const {
		/* Return a string
		 * presenting the object
		 */
		FString s{"<NODE|" + typeToString(node_type) + ">\n"};
		// value
		s += FString(value ? "Has a value" : "Has no value") + ".\n";

		//children
		if (childrenNamed) {
			for (const auto& pair : named_children) 
				s += pair.first + ": " + typeToString(pair.second.node_type) + "\n";
		} else {
			// the following cast is ugly, but silent the stupid -Wsign-compare warning
			// maybe I should have used decltype instead
			for (int i{0}; i < static_cast<int>(numbered_children.size()); ++i)
				s += FString(i) + ": " + typeToString(numbered_children.at(static_cast<size_t>(i)).node_type) + "\n";
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
