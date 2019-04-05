#ifndef SCHILDREN_H
#define SCHILDREN_H
/* Contains the Schildren class
 * which manages the children
 * of a Symbol
 */

#include <list>
#include <utility>
#include <vector>
#include "node.h"
#include "symbol.h"
#include "tailresult.h"
//#include "header.h"

namespace frumul {
	// forward declaration. Symbol is included in symbol.h
	class Symbol;
	class TailResult;

	class Schildren {
		/* Table of Symbol children
		 * of a Symbol
		 */
		public:
			enum PathFlag {
				/* Used when looking for a path
				 */
				No = 1 << 0, 		// basic: no option
				Relative = 1 << 1, 	// allows '§'
				Privileged = 1 << 2,	// with privileged parameter
				Parameter = 1 << 3,	// with parameter name WARNING there's a class in frumul namespace which is named Parameter too
			};
			Schildren ();
			Schildren (Symbol& np);
			// getters-setters
			Symbol& getChild(const Node& node);
			// getters
			Symbol& getChild(const FString& name) ;
			bool hasChildren() const;
			operator bool () const;
			bool hasChild(const FString& name) const;
			bool hasChild(const Node& node) const;
			bool hasParent() const;
			std::vector<Symbol*> sortChildrenByLongName();
			// finders

			TailResult find(const FString& path, const PathFlag flag=PathFlag::No) ;
			// const getters
			const Symbol& getChild(const FString& name) const;
			const std::vector<Symbol*> getChildren() const;
			// setters
			Symbol& addChild(const Symbol& s);
			Symbol& addChildReference(Symbol& s);
			Symbol& appendChild();
			Symbol& appendChild(const FString& name);

		private:
			Symbol* parent{nullptr};
			std::list<Symbol> children;
			std::vector<Symbol*> not_owned_children;
			// functions
			TailResult _findRestOfTail(const FString&,const PathFlag flag=PathFlag::No);
	};
}
#endif
