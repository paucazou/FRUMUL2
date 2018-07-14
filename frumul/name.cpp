#include <array>
#include "exception.h"
#include "name.h" 

namespace frumul {
	Name::Name ()
	{}

	void Name::addShort(const Node& node) {
		/* Add a short name
		 */
		const bst::str& ns {node.getValue()};

		if (ns.uLength() != 1)
			throw exc(exc::InvalidName,"A short name must have only one character",node.getPosition());

		if (sname) {
			if (sname != ns)
				throw iexc(
						exc::NameAlreadyDefined,
						bst::str("The short name associated with this long name has already been defined: ") + sname,
						node.getPosition(),
						"Association already defined here:",
						getBothPositions()
					);
		}
		else
			sname = ns;
		positions.emplace("short",node.getPosition());
	}

	void Name::addLong(const Node& node) {
		/* Add a long name
		 */
		const bst::str& nl {node.getValue()};

		if (nl.uLength() <= 1)
			throw exc(exc::InvalidName,"A long name must have at least two characters",node.getPosition());

		if (lname) {
			if (lname != nl)
				throw iexc(
						exc::NameAlreadyDefined,
						bst::str("The long name associated with this short name has already been defined: ") + lname,
						node.getPosition(),
						"Association defined here:",
						getBothPositions()
					  );
		}
		else
			lname = nl;
		positions.emplace("long",node.getPosition());
	}

	void Name::addBoth(const Node& node) {
		/* Add a long and a short name.
		 * Useful in a previous declaration
		 */
		addShort(node.get("short"));
		addLong(node.get("long"));
		positions.emplace("both",node.getPosition());
	}

	const bst::str& Name::getLong() const {
		/* Return the long name
		 * If no long name, return an empty
		 * value
		 */
		return lname;
	}

	const bst::str& Name::getShort() const {
		/* Return the short name.
		 * If no long name, return an empty
		 * value
		 */
		return sname;
	}

	bool Name::hasShort() const {
		/* true if it has
		 * a short name
		 */
		return sname;
	}

	bool Name::hasLong() const {
		/* true if it has
		 * a long name
		 */
		return lname;
	}

	Name::Filling contenance() const {
		/* Return an enum value
		 * matching with the names
		 * set in the object
		 */
		if (!(sname || lname))
			return NO;
		if (lname && !sname)
			return LONG_ONLY;
		if (sname && !lname)
			return SHORT_ONLY;
		return ALL;
	}

	PPosVect Name::getShortNamePositions() const {
		/* Return an array of const pointers
		 * to every position of the short names
		 * This array is static and does not reflect
		 * the evolution of the object.
		 */
		return getPositions("short");
	}

	PPosVect Name::getLongNamePositions() const {
		/* Idem for long names
		 */
		return getPositions("long");
	}

	PPosVect Name::getBothPositions() const {
		/* return positions for both names
		 */
		return getPositions("both");
	}

	PPosVect Name::getPositionsOf(const bst::str& type) const {
		/* Return the positions where type is found
		 */
		assert(!in<bst::str,std::array<bst::str,3>>(type,{"short","long","both"})
				&& "type must be short, long or both");

		PPosVect vect;
		for (const auto& pair : positions)
			if (pair.first == type)
				vect.push_back(pair.second);

		return vect;
	}

	StrPosMap& Name::getPositions() const {
		/* Return a reference
		 * to all positions
		 */
		return positions;
	}

	bst::str Name::toString() const {
		/* Create a representation of Name
		 */
		bst::str returned{"<Name>\n"};
		if (sname)
			returned += "Short: " + sname + '\n';
		if (lname)
			returned += "Long: " + lname + '\n';
		for (const auto& pair : positions)
			returned += pair.second.toString();
		return returned;
	}

	bool Name::operator && (const Name& other) const {
		/* return true if short AND long names both are equal
		 */
		return sname == other.sname && lname == other.lname;
	}

	bool Name::operator || (const Name& other) const {
		/* return true if short OR long name are equal
		 */
		return sname == other.sname || lname == other.lname;
	}

	std::ostream& operator<< (std::ostream& out, const Name& n) {
		/* Represent Name on stdout
		 */
		out << n.toString();
		return out;
	}

