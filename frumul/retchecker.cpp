#include <cassert>
#include "retchecker.h"

namespace frumul {

	RetChecker::RetChecker(int level,bool activated) :
	return_certified{!activated}
	{
		for (;level > 0; --level) {
			block_return.push(false);
		}
	}


	RetChecker::operator bool () const {
		/* true if the instance certifies
		 * that a return statement will
		 * be called
		 */
		return return_certified;
	}

	void RetChecker::deactivate() {
		/* Mark the return checker as
		 * deactivated.
		 * Every function will work as if a return
		 * statement has been found
		 */
		return_certified = true;
	}

	void RetChecker::set(bool b) {
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
				block_return.at(level) = b;
		}
	}

	bool RetChecker::pop() {
		/* Pop out the last element in the stack
		 * and change level
		 */
		if (block_return.size() == level)
			this->operator--();
		return return_certified ? return_certified : block_return.pop();
	}

	unsigned int RetChecker::operator ++ () {
		/* increments the level
		 * and push a false in the stack
		 * return current level
		 */
		++level;
		block_return.push(false);
		assert(level == block_return.size() && "Level and stack size don't match");
		return level;
	}

	unsigned int RetChecker::operator -- () {
		/* Decrements the level but does not
		 * pop the last value.
		 * Be careful.
		 * return current level
		 */
		assert(level > 0 && "Level is under zero");
		--level;
		return level;
	}

	unsigned int RetChecker::getCurrentLevel () const {
		return level;
	}






}