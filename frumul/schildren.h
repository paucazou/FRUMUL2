#ifndef SCHILDREN_H
#define SCHILDREN_H
/* Contains the Schildren class
 * which manages the children
 * of a Symbol
 */

#include <utility>
#include <vector>
#include "node.h"

namespace frumul {
	// forward declaration. Symbol is included in symbol.h
	class Symbol;

	class Schildren {
		/* Table of Symbol children
		 * of a Symbol
		 */
		public:
			Schildren ();
			Schildren (Symbol& np);
			// getters-setters
			Symbol& getChild(const Node& node);
			// getters
			Symbol& getChild(const bst::str& name) ;
			bool hasChildren() const;
			operator bool () const;
			bool hasChild(const bst::str& name) const;
			bool hasChild(const Node& node) const;
			bool hasParent() const;
			// const getters
			const Symbol& getChild(const bst::str& name) const;
			const std::vector<Symbol>& getChildren() const;
			// setters
			Symbol& addChild(const Symbol& s);
			Symbol& appendChild();
			Symbol& appendChild(const bst::str& name);
		private:
			Symbol* const parent{nullptr};
			std::vector<Symbol> children;
	};
}
#endif
