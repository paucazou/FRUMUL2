#include "warning.h"

namespace frumul {
	Warning::Warning (Type ntype, const bst::str& ninfo, const Position& np) :
		type{ntype}, info{ninfo}, pos{np}
	{
		std::cout << *this;
	}

	Warning::toString() const {
		/* string representation
		 */
		bst::str s{typeToString(type)};
		s += ": " + info + '\n';
		s += pos.toString();
		return s;
	}

	InconsistentWarning::InconsistentWarning(Type ntype, const bst::str& ninfo, const Position& npos, const bst::str& ninfo2, const std::vector<Position>& positions) :
		Warning::Warning(ntype,ninfo,npos), info2{ninfo2}
	{
		for (const auto& elt : npositions)
			positions.push_back(elt);
		std::cout << *this;
	}

	InconsistentWarning::toString() const {
		/* string representation
		 */
		bst::str s{Warning::toString()};
		s += info2 + '\n';
		for (const auto& elt : positions)
			s += elt.toString();
		return s;
	}
		
}
