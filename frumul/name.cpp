#include <array>
#include <cassert>
#include "name.h" 

namespace frumul {
	Name::Name ()
	{}

	Name::Name (const Name& other) {
		/* Copy constructor.
		 * Update the instance
		 */
		if (other.sname && !sname)
			sname = other.sname;
		else if (other.sname && other.sname != sname)
			throw iexc(exc::NameAlreadyDefined,"The short name has already been defined here:",getShortNamePositions(),"New short name defined here:",other.getShortNamePositions());

		if (other.lname && !lname)
			lname = other.lname;
		else if (other.lname && other.lname != lname)
			throw iexc(exc::NameAlreadyDefined,"The long name has already been defined here:",getLongNamePositions(),"New long name defined here:",other.getLongNamePositions());

		for (const auto& pair : other.positions)
			positions.emplace(pair.first,pair.second);
	}

	Name::Name (const Node& node) : Name(){
		/* Constructs a name with a node
		 */
		add(node);
	}

	void Name::add(const Node& node) {
		/* Add a new name
		 */
		switch(node.type()) {
			case Node::LINKED_NAMES:
				addBoth(node);
				break;
			case Node::SHORT_NAME:
				addShort(node);
				break;
			case Node::LONG_NAME:
				addLong(node);
				break;
			default:
				assert(false&&"Node expected: LINKED_NAMES, SHORT_NAME, LONG_NAME");
		};
	}

	void Name::add(const FString& name) {
		/* add a name, short or long
		 * WITHOUT position. Don't use it
		 * in the regular interpreter
		 */
		if (name.length() == 1) {
			if (!sname)
				sname = name;
			else
				assert(sname == name&&"Short name already set");
		}
		else
			if (!lname)
				lname = name;
			else
				assert(lname == name&&"Long name already set");
	}

	void Name::add(const FString& name, const Position& pos) {
		/* Add a name with a position
		 */
		add(name);
		const FString n = (name.length() == 1 ? "short" : "long");
		positions.emplace(n,pos);
	}


	void Name::addShort(const Node& node) {
		/* Add a short name
		 */
		const FString& ns {node.getValue()};

		if (ns.length() != 1)
			throw exc(exc::InvalidName,"A short name must have only one character",node.getPosition());

		if (sname) {
			if (sname != ns)
				throw iexc(
						exc::NameAlreadyDefined,
						FString("The short name associated with this long name has already been defined: ") + sname,
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
		const FString& nl {node.getValue()};

		if (nl.length() <= 1)
			throw exc(exc::InvalidName,"A long name must have at least two characters",node.getPosition());

		if (lname) {
			if (lname != nl)
				throw iexc(
						exc::NameAlreadyDefined,
						FString("The long name associated with this short name has already been defined: ") + lname,
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

	const FString& Name::getLong() const {
		/* Return the long name
		 * If no long name, return an empty
		 * value
		 */
		return lname;
	}

	const FString& Name::getShort() const {
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

	Name::Filling Name::contenance() const {
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

	PosVect Name::getShortNamePositions() const {
		/* Return an array of const pointers
		 * to every position of the short names
		 * This array is static and does not reflect
		 * the evolution of the object.
		 */
		return getPositionsOf("short");
	}

	PosVect Name::getLongNamePositions() const {
		/* Idem for long names
		 */
		return getPositionsOf("long");
	}

	PosVect Name::getBothPositions() const {
		/* return positions for both names
		 */
		return getPositionsOf("both");
	}

	PosVect Name::getPositionsOf(const FString& type) const {
		/* Return the positions where type is found
		 */
		bool isTypeRegular { !in<FString,std::array<FString,3>>(type,{{"short","long","both"}}) };
		assert(!isTypeRegular&& "type must be short, long or both");

		PosVect vect;
		for (const auto& pair : positions)
			if (pair.first == type)
				vect.push_back(pair.second);

		return vect;
	}

	const StrPosMap& Name::getPositions() const {
		/* Return a reference
		 * to all positions
		 */
		return positions;
	}

	FString Name::toString() const {
		/* Create a representation of Name
		 */
		FString returned{"<Name>\n"};
		if (sname)
			returned += "Short: " + sname + '\n';
		if (lname)
			returned += "Long: " + lname + '\n';
		for (const auto& pair : positions)
			returned += pair.second.toString();
		return returned;
	}

	FString Name::names() const {
		/* Simple representation of the names
		 */
		if (sname&&lname)
			return sname + " - " + lname;
		return (sname ? sname : lname);
	}

	bool Name::operator == (const FString& name) const {
		/* true if name is one of the two names
		 */
		return (name == sname || name == lname);
	}

	bool Name::operator && (const Name& other) const {
		/* return true if short AND long names both are equal
		 */
		return (sname == other.sname && lname == other.lname);
	}

	bool Name::operator || (const Name& other) const {
		/* return true if short OR long name are equal
		 */
		return ((sname ? (sname == other.sname) : false) ||
			(lname ? (lname == other.lname) : false));
	}

	std::ostream& operator<< (std::ostream& out, const Name& n) {
		/* Represent Name on stdout
		 */
		out << n.toString();
		return out;
	}
}

