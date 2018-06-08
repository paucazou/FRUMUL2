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
                        std::cerr << last_exception.what();
                }
                else {
                        std::cerr << "Unexpected error.\n";
                }
        }
	
	//classes
	BaseException::BaseException (Type t, const bst::str& ninfo, const Position& npos) :
		type{t}, addinfo{ninfo}, pos{npos}
	{}

	virtual const bst::str what () const noexcept {
		bst::str returned{types.at(static_cast<int>(type))};
		returned += ": " + addinfo + '\n';
		returned += pos.toString();
		return returned;
	}
	BaseException::types {"SyntaxError",};





}// namespace
