#ifndef EXCEPTION_H
#define EXCEPTION_H

/* Manages the exceptions
 * They are destined
 * to the end user of the program.
 */
#include "position.h"
#include <iostream>

namespace frumul {
	// functions
	extern void terminate ();

	// classes
	class BaseException {
		/* Base class of all exceptions
		 */
		public:
			enum Type { // exception type
				SyntaxError,
				UnexpectedToken,
				MAX_TYPES,
			};

			BaseException (Type ntype,const bst::str& ninfo,const Position& npos);
			virtual const bst::str what () const noexcept; // return a custom message
		protected:
			const Type type;
			const bst::str addinfo;		// infos entered while creating the instance
			const Position pos;		// where the error has occured
			static const std::array<bst::str,MAX_TYPES> types; // names of the exceptions
	};

}//namespace

#endif
