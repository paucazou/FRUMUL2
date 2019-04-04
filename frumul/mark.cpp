#include "mark.h"

namespace frumul {

	// constructors
	Mark::Mark ()
	{}

	Mark::Mark (const Mark& other) {
		/* This copy constructor actually
		 * set the value
		 * and append the positions
		 */
		if (!val)
			val = other.val;
		else if (val != other.val)
			throw iexc(exc::InconsistentMark,"Mark does not match with previous mark settings",other.positions,"Previous mark settings here:",positions);

		for (const auto& elt : other.positions)
			positions.push_back(elt);
	}

	void Mark::set (const Node& node) {
		/* set the value
		 * if necessary
		 * and add position
		 */
		int nval {static_cast<int>(node.getValue())};

		if (nval <= 0)
			throw exc(exc::InvalidMark,"Mark must be equal or above one",node.getPosition());

		if (!val) // val not yet set
			val = nval;

		if (val != nval)
			throw iexc(exc::InconsistentMark,"Mark does not match with previous Mark set",node.getPosition(),"Mark already set here:",positions);

		positions.push_back(node.getPosition());
	}

	void Mark::set (const Mark& other) {
		/* set the value
		 * if it has not been set before
		 */
		assert(val <= 0 && "Mark value has already been set");
		val = other.val;
		for (const auto& pos : other.positions)
			positions.push_back(pos);
	}

	void Mark::set (int i) {
		/* Set value. Should not be called without
		 * care
		 */
		assert(val <= 0 && "Mark value has already been set");
		val = i;
	}

	void Mark::operator() (const Node& node) {
		/* Same that set
		 */
		set(node);
	}

	int Mark::get() const {
		/* Return the value
		 */
		return val;
	}

	int Mark::operator () () const {
		/* Return the value.
		 * A zero value
		 * means that it is not yet set
		 */
		return val;
	}

	int Mark::afterArgsNumber() const {
		/* Return the number of tags expected after each arg.
		 * It is equal to the number of arguments expected
		 */
		return val - 1;
	}

	const PosVect& Mark::getPositions() const {
		return positions;
	}

	FString Mark::toString() const {
		/* Representation of the Mark
		 */
		FString returned {"<Mark|"};
		returned += val;
		returned += ">\n";
		for (const auto& elt : positions)
			returned += elt.toString();

		return returned;
	}

	std::ostream& operator<<(std::ostream& out, const Mark& m) {
		/* Print Mark in stdout
		 */
		out << m.toString();
		return out;
	}

}
