/* Contains the Schildren class
 * which manages the children
 * of a Symbol
 */

#include <vector>
#include "node.h"
#include "symbol.h"

namespace frumul {
	class Schildren {
		/* Table of Symbol children
		 * of a Symbol
		 */
		public:
			Schildren ();
			Schildren (const Symbol& np);
			// getters-setters
			Symbol& getChild(const Node& node);
			// getters
			Symbol& getChild(const bst::str& name) const;
			bool hasChildren() const;
			bool hasChild(const bst::str& name) const;
			bool hasChild(const Node& node) const;
			bool hasParent() const;
			// setters
			Symbol& addChild(const Symbol& s);
			Symbol& appendChild();
			Symbol& appendChild(const bst::str& name);
		private:
			Symbol* const parent{nullptr};
			std::vector<Symbol> children;
	};
}
