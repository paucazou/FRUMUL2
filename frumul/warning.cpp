#include "warning.h"

namespace frumul {
	Warning::Warning (Type ntype, const bst::str& ninfo, const Position& np) :
		type{ntype}, info{ninfo}, pos{np}
	{
		std::cout << *this;
	}

	Warning::Warning (Type ntype, const bst::str& ninfo) :
		type{ntype}, info{ninfo}, pos{-1,-1,"No","No"}
	{
		/* Protected constructor used for derived classes only
		 * Does not print the instance and creates a non safe position
		 */
	}

	bst::str Warning::toString() const {
		/* string representation
		 */
		bst::str s{typeToString(type)};
		s += ": " + info + '\n';
		s += pos.toString();
		return s;
	}

	InconsistentWarning::InconsistentWarning(Type ntype, const bst::str& ninfo, const Position& npos, const bst::str& ninfo2, const std::vector<Position>& npositions) :
		Warning::Warning(ntype,ninfo), positions1{{npos}}, info2{ninfo2}, positions2{npositions}
	{
		std::cout << *this;
	}

	InconsistentWarning::InconsistentWarning(Type ntype, const bst::str& ninfo1, const std::vector<Position> npos1, const bst::str& ninfo2, const std::vector<Position> npos2):
	Warning::Warning(ntype,ninfo1), positions1{npos1}, info2{ninfo2}, positions2{npos2}
	{
		std::cout << *this;
	}

	InconsistentWarning::InconsistentWarning(Type ntype, const bst::str& ninfo, const Position& npos1, const bst::str& ninfo2, const Position& npos2) :
		Warning::Warning(ntype,ninfo), positions1{{npos1}}, info2{ninfo2}, positions2{{npos2}}
	{
		std::cout << *this;
	}

	InconsistentWarning::InconsistentWarning(Type ntype, const bst::str& ninfo, const std::vector<Position>& npos) :
		Warning::Warning(ntype,ninfo), positions1{npos}
	{
		std::cout << *this;
	}

	bst::str InconsistentWarning::toString() const {
		/* string representation
		 */
		bst::str s{typeToString(type)};
		s += ": " + info + '\n';
		for (const auto& elt : positions1)
			s += elt.toString();
		s += info2 + '\n';
		for (const auto& elt : positions2)
			s += elt.toString();
		return s;
	}
		
}
