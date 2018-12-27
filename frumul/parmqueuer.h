#ifndef PARMQUEUER_H
#define PARMQUEUER_H

#include <unordered_map>
#include <vector>
#include "parameters.h"
#include "position.h"
namespace frumul {
	struct Arg;
	using ParmVect = std::vector<Parameter>;
	using CRParmVect = std::vector<CRParameter>;

	class ParmQueuer {
		/* Functor that manages the queue
		 * of the parameters when a symbol is called
		 */
		public:
			explicit ParmQueuer(ParmVect&, const bst::str& lang);
			Parameter& operator () (const Arg&);
			Parameter& operator () (const bst::str& val, const Position& pos, const bst::str& name = "");
			bool areParametersFilled () const;
		private:
			// attributes
			unsigned int pos{0};
			ParmVect& parms;
			std::vector<bool> checked;
			const bst::str& lang;
			std::unordered_map<CRParameter,unsigned int> call_number;
			// functions
			Parameter& select_parm(const Arg&);
			

	};
}
#endif
