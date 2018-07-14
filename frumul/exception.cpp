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
	InconsistantException::InconsistantException (Type ntype, const bst::str& ninfo, const Position& npos, const bst::str& ninfo2, const std::vector<const Position*> npositions) :
		BaseException(ntype,ninfo,npos), info2{ninfo2}
	{
		for (const auto elt : npositions)
			positions.push_back(elt);
	}

	const bst::str InconsistantException::what () const noexcept {
		const bst::str base_what {BaseException::what()};
		bst::str returned{base_what};
		returned += info2;
		for (const auto& elt : positions)
			returned += elt.toString();
		return returned;
	}
	}





}// namespace
