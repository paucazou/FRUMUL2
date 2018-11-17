#include "retchecker.h"

namespace frumul {
	RetChecker::operator bool () const {
		/* true if the instance certifies
		 * that a return statement will
		 * be called
		 */
		return return_certified;
	}

	void RetChecker::push(bool b) {
		/* Certify that the current block
		 * return or not
		 * Nothing is done if a return statement
		 * has been certified.
		 */
		if (!return_certified) {
			if (block_return.empty() && b) {
				// in this case, the return statement is certified
				return_certified = true;
			} else
				block_return.push(b);
		}
	}

	bool RetChecker::pop() {
		/* Pop out the last element in the stack
		 */
		return block_return.pop();
	}

}
