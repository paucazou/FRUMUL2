#ifndef NODE_H
#define NODE_H

#include <map>
#include <iostream>
#include <vector>
#include "macros.h"
#include "position.h"

#define LIST_NODES(NOD) \
	NOD(DOCUMENT) \
	NOD(EMPTY) \
	NOD(HEADER) \
	NOD(STATEMENT_LIST) \
	NOD(DECLARATION) \
	\
	NOD(BASIC_VALUE) \
	NOD(NAMESPACE_VALUE)\
	NOD(VARIABLE_DECLARATION)\
	NOD(VARIABLE_ASSIGNMENT)\
	NOD(BIN_OP)\
	NOD(UNARY_OP)\
	NOD(COMPARE_OP)\
	NOD(COMPARISON)\
	NOD(VAL_TEXT) \
	NOD(LOOP)\
	NOD(CONDITION)\
	\
	NOD(LONG_NAME)\
	NOD(SHORT_NAME)\
	NOD(LINKED_NAMES)\
	\
	NOD(OPTIONS) \
	NOD(MARK) \
	NOD(LANG) \
	NOD(PARAM)\
	\
	NOD(VARIABLE_NAME)\
	NOD(VARIABLE_TYPE)\
	NOD(LITTEXT)\
	NOD(LITINT)\
	NOD(LITBOOL)\
	NOD(LIST)\
	\
	NOD(TEXT) \
	NOD(SIMPLE_TEXT) \
	NOD(MAX_TYPES) \



namespace frumul {
	class Node {
		/* AST Node
		 */
		public:
			ENUM(Type,LIST_NODES)
			
			// constructors
			Node (const Type ntype, const Position& npos, const std::map<bst::str,Node>& nattr,const bst::str& nvalue);
			Node (const Type ntype, const Position& npos, const std::vector<Node>& nattr, const bst::str& nvalue);

			Node (const Type ntype, const Position& npos, const bst::str& nvalue);

			Node (const Type ntype, const Position& npos, const std::map<bst::str,Node>& nattr);
			Node (const Type ntype, const Position& npos, const std::vector<Node>& nattr={});

			Node (const Node& n);

			// destructor
			~Node();

			//getters
			Type type() const;
			bool areChildrenNamed () const;

			const Position& getPosition() const;

			const bst::str& getValue() const;

			const Node& get(const bst::str& key) const;
			const Node& get(int index) const;

			const std::map<bst::str,Node>& getNamedChildren() const;
			const std::vector<Node>& getNumberedChildren() const;
			
			//setters
			void addChild(const bst::str& name, const Node& child);
			void addChild(const Node& child);

			// operators
			void operator=(const Node& n);

			// display functions
			const bst::str toString() const;

			friend std::ostream& operator<< (std::ostream& out, const Node& n);
		private:
			const Type node_type;
			const Position pos;
			union {
				std::map<bst::str,Node> named_children;
				std::vector<Node> numbered_children;
			};
			const bst::str value{""};
			const bool childrenNamed {false};

	};

	// two alias for widely used templates types
	using NodeVector = std::vector<Node>;
	using StrNodeMap = std::map<bst::str,Node>;

}// namespace
#endif
