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
	E(InvalidName)\
	E(NoForwardDeclaration)\
	E(RequiredOptionNotSet)\
	E(NameAlreadyDefined)\
	E(SymbolNotFound)\
	E(AliasAlreadySet)\
	E(InvalidMark)\
	E(InconsistentMark)\
	E(UnknownType)\
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

	class InconsistantException : public BaseException {
		public:
			InconsistantException (Type ntype, const bst::str& ninfo,const Position& npos, const bst::str& ninfo2, const std::vector<Position> npositions);
			//InconsistantException (Type ntype, const bst::str ninfo,const Position& npos, const bst::str ninfo2, const std::vector<Position> npositions);
			InconsistantException (Type ntype, const bst::str& ninfo, const std::vector<Position> npos1, const bst::str& ninfo2, const std::vector<Position> npositions2);

			InconsistantException (Type ntype, const bst::str& ninfo1, const Position& npos1, const bst::str& ninfo2, const Position& npos2);

			virtual const bst::str what () const noexcept;
		protected:
			std::vector<Position> positions1;
			const bst::str info2; // this info will be print just before 'positions2'
			std::vector<Position> positions2;
	};

	using exc = BaseException;
	using iexc = InconsistantException;

}//namespace

#endif
