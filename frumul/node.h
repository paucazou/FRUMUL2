#ifndef NODE_H
#define NODE_H

#include <map>
#include <iostream>
#include "position.h"

namespace frumul {
	class Node {
		/* AST Node
		 */
		public:
			enum Type {
				// document
				DOCUMENT,
				EMPTY,
				//header
				HEADER,
				STATEMENT_LIST,
				//text
				TEXT,
				SIMPLE_TEXT,
				MAX_TYPES,
			};

			Node (const Type ntype, const Position& npos, const std::map<bst::str,Node>& nattr = {});
			Node (const Node& n);

			const Position& getPosition() const;
			const Node& get(const bst::str& key) const;
			void addChild(const bst::str& name, const Node& child);
			Type type() const;

			const bst::str toString() const;
			static bst::str typeToString (Type t);

			friend std::ostream& operator<< (std::ostream& out, const Node& n);
		private:
			const Type node_type;
			const Position pos;
			std::map<bst::str,Node> children;

	};

}// namespace
#endif
