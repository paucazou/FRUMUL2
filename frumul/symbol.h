/* This file contains the Symbol
 * definition
 */

#include <memory>
#include "name.h"
#include "mark.h"
#include "node.h"
#include "position.h"
#include "schildren.h"

namespace frumul {

	class Symbol {
		public:
			Symbol ();
			Symbol(const Symbol& other);
			//getters
			Name& getName();
			Mark& getMark();
			Schildren& getChildren();
			// booleans
			bool hasParent() const;
			bool hasAlias () const;
			bool hasMark () const;
			bool hasChildren () const;

		protected:
			Name name {};
			Mark mark {};
			std::unique_ptr<Schildren> children;
			Symbol* parent {nullptr};
			Symbol* alias {nullptr};
			// values
			// langs ?
			// children
			// generic value ?

	};
}
