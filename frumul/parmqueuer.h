#ifndef PARMQUEUER_H
#define PARMQUEUER_H

#include <functional>
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
			explicit ParmQueuer(ParmVect&, const FString& lang);
			Parameter& operator () (const Arg&);
			Parameter& operator () (const FString& val, const Position& pos, const FString& name = "");
			bool areParametersFilled () const;
			bool areNonDefaultParametersFilled() const;
			bool hasUnfilledDefault() const;
			void markFinished(const Parameter&);
			std::vector<std::reference_wrapper<Parameter>> getUnfilledDefault();

		private:
			// attributes
			unsigned int pos{0};
			ParmVect& parms;
			std::vector<bool> checked;
			const FString& lang;
			std::unordered_map<CRParameter,unsigned int> call_number;
			// functions
			Parameter& select_parm(const Arg&);
			

	};
}
#endif
