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
	NOD(VAL_TEXT) \
	\
	NOD(OPTIONS) \
	NOD(MARK) \
	NOD(LANG) \
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
			Node (const Type ntype, const Position& npos, const std::map<bst::str,Node>& nattr = {},const bst::str& nvalue = "");
			Node (const Type ntype, const Position& npos, const std::vector<Node>& nattr = {}, const bst::str& nvalue = "");
			Node (const Node& n);

			//getters
			Type type() const;
			bool areChildrenNamed () const;

			const Position& getPosition() const;

			const Node& get(const bst::str& key) const;
			const Node& get(int index) const;
			
			//setters
			void addChild(const bst::str& name, const Node& child);
			void addChild(const Node& child);


			// display functions
			const bst::str toString() const;

			friend std::ostream& operator<< (std::ostream& out, const Node& n);
		private:
			const Type node_type;
			const Position pos;
			const bst::str value;
			union {
				std::map<bst::str,Node> named_children;
				std::vector<Node> numbered_children;
			};
			const bool childrenNamed {true};

	};

}// namespace
#endif
