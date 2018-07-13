#ifndef EXCEPTION_H
#define EXCEPTION_H

/* Manages the exceptions
 * They are destined
 * to the end user of the program.
 */
#include <iostream>
#include "macros.h"
#include "position.h"

#define EXCEPTIONS_LIST(E) \
	E(SyntaxError)\
	E(UnexpectedToken)\
	E(TagNotFound)\
	E(FileError)\
	E(MAX_TYPES)\

namespace frumul {
	// functions
	extern void terminate ();

	// classes
	class BaseException {
		/* Base class of all exceptions
		 */
		public:
			ENUM(Type,EXCEPTIONS_LIST)

			BaseException (Type ntype,const bst::str& ninfo,const Position& npos);
			virtual const bst::str what () const noexcept; // return a custom message
		protected:
			const Type type;
			const bst::str addinfo;		// infos entered while creating the instance
			const Position pos;		// where the error has occured
	};
	using exc = BaseException;

}//namespace

#endif
