#ifndef PARMQUEUER_H
#define PARMQUEUER_H

#include <unordered_map>
#include <vector>
#include "parameters.h"
namespace frumul {
	struct Arg;
	using CParmVect = const std::vector<Parameter>;
	using CRParmVect = std::vector<CRParameter>;

	class ParmQueuer {
		/* Functor that manages the queue
		 * of the parameters when a symbol is called
		 */
		public:
			explicit ParmQueuer(CParmVect&, const bst::str& lang);
			const Parameter& operator () (const Arg&);
		private:
			unsigned int pos{0};
			CParmVect& parms;
			std::vector<bool> checked;
			const bst::str& lang;
			std::unordered_map<CRParameter,unsigned int> call_number;
			

	};
}
#endif
