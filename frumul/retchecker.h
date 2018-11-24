#ifndef RETCHECKER_H
#define RETCHECKER_H
#include "functions.inl"

namespace frumul {
	class RetChecker {
		/* This class has the charge
		 * to check that a value
		 * has returned something
		 * or not
		 * How to use:
		 * at the start of each level (block),
		 * operator ++ should be called,
		 * and operator -- at the end of the block.
		 * last value should be popped later to handle
		 * the information.
		 */
		public:
			explicit RetChecker(bool activated=true, int level = 0);
			operator bool() const;
			
			void deactivate();
			void set(bool);
			bool pop();
			unsigned int operator++();
			unsigned int operator--();
			unsigned int getCurrentLevel() const;
			
		private:
			bool return_certified{false};
			unsigned int level{0};
			rstack<bool> block_return;
	};
}
#endif
