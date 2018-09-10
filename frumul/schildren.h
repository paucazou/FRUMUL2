#ifndef SCHILDREN_H
#define SCHILDREN_H
/* Contains the Schildren class
 * which manages the children
 * of a Symbol
 */

#include <utility>
#include <vector>
#include "node.h"
#include "symbol.h"
//#include "header.h"

namespace frumul {
	// forward declaration. Symbol is included in symbol.h
	class Symbol;
	enum PathFlag {
		/* Used when looking for a path
		 */
		No = 1 << 0, 		// basic: no option
		Relative = 1 << 1, 	// allows '§'
		Privileged = 1 << 2,	// with privileged parameter
		Parameter = 1 << 3,	// with parameter name
	};

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
			// finders

			const Symbol& find(const bst::str& path, PathFlag flag=PathFlag::No) const;
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
