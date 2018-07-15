/* This file contains
 * the Mark class
 */

#include <vector>
#include "node.h"
#include "position.h"


namespace frumul {
	using PosVect = std::vector<Position>;

	class Mark {
		public:
			Mark();
			// add a value
			void set (const Node& node);
			void operator (const Node& node);
			// get the value
			int get () const;
			int operator ()() const;
			// representation
			bst::str toString() const;
			friend operator<<(std::ostream& out,const Mark& m);
		private:
			int val{0};
			PosVect positions;
	};
}

