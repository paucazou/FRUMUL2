#ifndef MARK_H
#define MARK_H
/* This file contains
 * the Mark class
 */

#include <cassert>
#include <vector>
#include "exception.h"
#include "node.h"
#include "position.h"

//#include "header.h"


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
			int afterArgsNumber() const;
			// representation
			const PosVect& getPositions() const;
			bst::str toString() const;
			friend std::ostream& operator<<(std::ostream& out,const Mark& m);
		private:
			int val{0};
			PosVect positions {};
			// private functions
	};
}
#endif
