#ifndef MARK_H
#define MARK_H
/* This file contains
 * the Mark class
 */

#include <cassert>
#include <vector>
#include "node.h"
#include "position.h"


namespace frumul {
	using PosVect = std::vector<Position>;

	class Mark {
		public:
			Mark();
			Mark(const Mark& other);
			// add a value
			void set (const Node& node);
			void set (const Mark& other);
			void operator ()(const Node& node);
			// get the value
			int get () const;
			int operator ()() const;
			// representation
			bst::str toString() const;
			friend std::ostream& operator<<(std::ostream& out,const Mark& m);
		private:
			int val{0};
			PosVect positions {};
			// private functions
	};
}
#endif
