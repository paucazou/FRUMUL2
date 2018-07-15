#include "exception.h"
#include "mark.h"

namespace frumul {
	Mark::Mark ()
	{}

	void Mark::set (const Node& node) {
		/* set the value
		 * if necessary
		 * and add position
		 */
		int nval {static_cast<int<(node.getValue())};

		if (nval <= 0)
			throw exc(exc::InvalidMark,"Mark must be equal or above one",node.getPosition());

		if (!val) // val not yet set
			val = nval;

		if (val != nval)
			throw iexc(exc::InconsistentMark,"Mark does not match with previous Mark set",node.getPosition(),"Mark already set here:",positions);

		positions.push_back(node.getPosition());
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

	bst::str Mark::toString() const {
		/* Representation of the Mark
		 */
		bst::str returned {"<Mark|"};
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
