#include <cassert>
#include "parmqueuer.h"
#include "exception.h"
#include "functions.inl"
#include "macros.h"
#include "vm.h"

namespace frumul {
	ParmQueuer::ParmQueuer (CParmVect& p, const bst::str& nlang) :
	parms{p}, lang{nlang}
	{
		/* Constructor
		 */
		for (size_t i{0}; i < parms.size(); ++i)
			checked.push_back(false);

	}

	const Parameter& ParmQueuer::operator() (const Arg& arg) {
		/* Return the next parameter
		 * matching the arg
		 */
		assert(pos < parms.size()&&"Pos is too large");
		// get the parameter

		const Parameter& parm { select_parm(arg) };
		CRParameter crparm{parm};
		
		// check number
		const auto defaultSetMap { &defaultSet<std::unordered_map<CRParameter,unsigned int>,CRParameter,unsigned int> };

		unsigned int call_nb { defaultSetMap(call_number,crparm,0) };
		if (call_nb > static_cast<unsigned int>(parm.getMax(lang)))
			throw iexc(exc::ArgumentNBError,"Too many arguments entered for the required parameter",arg.pos,"Parameter defined here: ",parm.getPositions());


		return parm;
	}

	const Parameter& ParmQueuer::select_parm(const Arg& arg) {
		/* Select a parameter
		 * and return it
		 */
#pragma message "Call with multiple args for one parameter not yet ready"
		// with a name
		if (arg.name) {
			for (size_t i{0}; i < parms.size(); ++i) {
				const auto& p { parms[i] };
				if (p.getName() == arg.name) {
					checked[i] = true;
					return p;
				}
			}
			throw exc(exc::NameError,bst::str("No parameter with name '") + arg.name + "'",arg.pos);
		}
		// with no name
		// is the parameter already checked ?
		try {
			while (checked.at(pos))
				++pos;
		} catch (std::out_of_range& e) {
			throw exc(exc::ArgumentNBError,"Too many arguments were entered",arg.pos);
		}
		// check the parameter
		checked.at(pos);
		return parms.at(pos++);
	}


}
