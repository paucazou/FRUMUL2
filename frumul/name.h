#ifndef NAME_H
#define NAME_H
/* This file
 * manages the Name class,
 * which represents the long and
 * short names of a symbol
 */
#include <vector>
#include <unordered_map>
#include "bstrlib/util.inl"
#include "exception.h"
#include "functions.inl"
#include "macros.h"
#include "node.h"
#include "position.h"
//#include "header.h"

namespace frumul {
	using StrPosMap = std::unordered_multimap<bst::str,Position>;
	using PosVect= std::vector<Position>;

	class Name {
		/* Class
		 * containing the names
		 * of a symbol
		 */
		public:
			enum Filling {
				NO,
				LONG_ONLY,
				SHORT_ONLY,
				ALL,
				MAX_FILLING,
			};
			// constructors
			Name ();
			Name (const Name& other);
			Name (const Node& node);
			// setters
			void add(const Node& node);
			void add(const bst::str& name);
			void addShort(const Node& node);
			void addLong(const Node& node);
			void addBoth(const Node& node);
			//getters
			const bst::str& getLong() const;
			const bst::str& getShort() const;
			bool hasShort() const;
			bool hasLong() const;
			Filling contenance () const;
			PosVect getShortNamePositions () const;
			PosVect getLongNamePositions () const;
			PosVect getBothPositions () const;
			PosVect getPositionsOf(const bst::str& type) const;
			const std::unordered_multimap<bst::str,Position>& getPositions() const;
			//display
			bst::str toString() const;
			bst::str names() const;
			//overload
			bool operator == (const bst::str& name) const;
			// operator && returns true if short and long names are equal
			bool operator && (const Name& other) const;
			// operator || returns true if at least one of the name is equal to one of the other
			bool operator || (const Name& other) const;
			friend std::ostream& operator<< (std::ostream& out,const Name& n);
		private:
			bst::str sname{""};
			bst::str lname{""};
			std::unordered_multimap<bst::str,Position> positions;
	};
} // namespace
#endif
