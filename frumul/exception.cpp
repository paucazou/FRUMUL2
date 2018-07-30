#include "exception.h"

namespace frumul {
	// functions
	void terminate () {
		/* Function which must be called
                 * if an exception has not been caught.
                 * She must be set with std::set_terminate;
                 */
                // Thanks: https://akrzemi1.wordpress.com/2011/10/05/using-stdterminate/
                if (const auto& last_exception = std::current_exception() ) {
			try
			{
				rethrow_exception(last_exception);
			}
		       	catch (const BaseException& exc) {
				std::cerr << exc.what();
			}
                }
                else {
                        std::cerr << "Unexpected error.\n";
                }
        }
	
	//classes
	//Base class
	BaseException::BaseException (Type ntype, const bst::str& ninfo, const Position& npos) :
		type{ntype}, addinfo{ninfo}, pos{npos}
	{}

	const bst::str BaseException::what () const noexcept {
		bst::str returned{typeToString(type)};
		returned += ": " + addinfo + '\n';
		returned += pos.toString();
		return returned;
	}

	// Inconsistant class
	InconsistantException::InconsistantException (Type ntype, const bst::str& ninfo, const Position& npos, const bst::str& ninfo2, const std::vector<Position> npositions) :
		BaseException(ntype,ninfo,npos), info2{ninfo2}, positions2{npositions}
	{
		positions1.push_back(npos);
	}

	/*InconsistantException::InconsistantException (Type ntype, const bst::str ninfo, const Position& npos, const bst::str ninfo2, const std::vector<Position> npositions) :
		BaseException(ntype,ninfo,npos), info2{ninfo2}, positions2{npositions}
	{
		positions1.push_back(npos);
	}
	*/

	InconsistantException::InconsistantException (Type ntype, const bst::str& ninfo, const std::vector<Position> npos1, const bst::str& ninfo2, const std::vector<Position> npositions2) :
		BaseException(ntype,ninfo,npos1.at(0)), positions1{npos1}, info2{ninfo2}, positions2{npositions2}
	{
		/* This constructor implies that the first vector
		 * has at least one value inside
		 */
	}

	const bst::str InconsistantException::what () const noexcept {
		bst::str returned{typeToString(type)};
		returned += ": " + addinfo + '\n';
		for (const auto& elt : positions1)
			returned += elt.toString();
		returned += info2 + '\n';
		for (const auto& elt : positions2)
			returned += elt.toString();
		return returned;
	}


}// namespace
