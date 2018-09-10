#ifndef FDECLARATION_H
#define FDECLARATION_H
/* This contains the Fdeclaration (forward declaration)
 * used with namespace values
 */
#include <vector>
#include "exception.h"
#include "name.h"
#include "symbol.h"
#include "warning.h"

//#include "header.h"

namespace frumul {
	class Fdeclaration {
		/* Contains the forward declaration
		 * of names inside namespace
		 */
		public:
			Fdeclaration(Symbol& ns);
			void newName(const Node&);
			void declareReady();
			operator bool () const;
			bool isReady() const;
			std::vector<Name> notUsed() const;
			bool match(const Node&);
		private:
			std::vector<Name> names;
			std::vector<bool> used;
			Symbol& namespac;
	};
}
#endif
