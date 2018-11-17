#ifndef RETCHECKER_H
#define RETCHECKER_H
#include "functions.inl"

namespace frumul {
	class RetChecker {
		/* This class has the charge
		 * to check that a value
		 * has returned something
		 * or not
		 */
		public:
			operator bool() const;
			
			void push(bool);
			//void set(bool);
			bool pop();
			
		private:
			bool return_certified{false};
			rstack<bool> block_return{{false}};
	};
}
#endif
