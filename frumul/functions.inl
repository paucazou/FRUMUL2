/* This file contains only
 * function templates
 */

namespace frumul {

	template <typename T>
	bool intersection(const T& first, const T& second) {
		/* Check wether at least one element of first
		 * is in second.
		 */
		for (const auto & elt: first)
			for (const auto & item : second)
				if (elt == item)
					return true;
		return false;
	}

	template <typename ELT, typename ITERABLE>
	bool in (const ELT& elt, const ITERABLE& iterable) {
		/* Checks wether the elt 
		 * is in iterable
		 */
		for (const auto& item : iterable)
			if (elt == item)
				return true;
		return false;
	}
}//namespace
