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
	BaseException::BaseException (Type ntype, const bst::str& ninfo, const Position& npos) :
		type{ntype}, addinfo{ninfo}, pos{npos}
	{}

	const bst::str BaseException::what () const noexcept {
		bst::str returned{types.at(static_cast<int>(type))};
		returned += ": " + addinfo + '\n';
		returned += pos.toString();
		return returned;
	}
	const std::array<bst::str,BaseException::MAX_TYPES> BaseException::types {"SyntaxError",};





}// namespace
