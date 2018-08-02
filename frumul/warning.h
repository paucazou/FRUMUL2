/* This file defines the warnings
 * used by the interpreter
 */
#ifndef WARNING_H
#define WARNING_H

#include <iostream>
#include <vector>
#include "macros.h"
#include "position.h"

#define WARNINGS_LIST(W) \
	W(InconsistentOptions)\
	W(NamesSetTwice) \
	W(MAX_TYPES)\

namespace frumul {
	class Warning {
		/* Warning base class
		 */
		public:
			ENUM(Type,WARNINGS_LIST)
			Warning (Type ntype,const bst::str& ninfo, const Position& np);
			// display
			virtual bst::str toString() const;
			STDOUT(Warning)
		protected:
			Type type;
			const bst::str info;
			const Position pos;
	};

	class InconsistentWarning : public Warning {
		/* Warning to be used
		 * when comparing two or more things
		 * in the code
		 */
		public:
			InconsistentWarning(Type ntype,const bst::str& ninfo,const Position& npos, const bst::str& ninfo2, const std::vector<Position>& npositions);
			InconsistentWarning(Type ntyp, const bst::str& ninfo, const std::vector<Position> npos1, const bst::str& ninfo2, const std::vector<Position> npos2);
			InconsistentWarning(Type ntype,const bst::str& ninfo,const Position& npos, const bst::str& ninfo2, const Position& npos2);
			//display
			virtual bst::str toString() const;
			STDOUT(InconsistentWarning)
		private:
			std::vector<Position> positions1;
			const bst::str info2;
			std::vector<Position> positions2;
	};
	using W = Warning;
	using IW = InconsistentWarning;
}

#endif
