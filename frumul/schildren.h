/* Contains the Schildren class
 * which manages the children
 * of a Symbol
 */

#include <vector>
#include "symbol.h"

namespace frumul {
	class Schildren {
		/* Table of Symbol children
		 * of a Symbol
		 */
		public:
			Schildren ();
			Schildren (const Symbol& np);
			//getters/setters
			//These functions set if the can't get
			Symbol& getChild(const bst::str& name);
			Symbol& getChild(const bst::str& sn, const bst::str& ln);
			Symbol& getChild(const Name& name);
			// pure getters
			bool hasChildren() const;
			// pure setters
			Symbol& addChild(const Symbol& s);
			Symbol& appendChild();
			Symbol& appendChild(const bst::str& name);
		private:
			Symbol* const parent{nullptr};
			std::vector<Symbol> children;
	};
}
