/* This file contains the Symbol
 * definition
 */

#include <memory>
#include "name.h"
#include "mark.h"
#include "node.h"
#include "position.h"
#include "schildren.h"
#include "value.h"

namespace frumul {

	class Symbol;

	class Alias {
		/* Simple class which handles
		 * a pointer to another Symbol
		 * and a Position
		 */
		public:
			Alias();
			operator bool () const; // true if it has a valid alias
			const Position& getPosition() const;
			const Symbol& getVal() const;
			void setPath(const Node& node);
			void setVal(const Symbol& nalias);
		protected:
			Symbol* val{nullptr};
			bst::str path;
			Position pos;
	};

	class Symbol {
		public:
			Symbol ();
			//getters
			Name& getName();
			Mark& getMark();
			Alias& getAlias();
			Schildren& getChildren();
			Value& getValue();
			// booleans
			bool hasParent() const;
			bool hasAlias () const;
			bool hasMark () const;
			bool hasChildren () const;
			bool hasValue() const;

		protected:
			Name name {};
			Mark mark {};
			Schildren children;
			Symbol* parent {nullptr};
			Alias alias;
			Value value;

	};
}
