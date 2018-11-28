#include "parmqueuer.h"
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
		// get the parameter
		const Parameter& parm { parms.at(pos) };
		CRParameter crparm{parm};
		checked.at(pos++) = true;
		
		// check number
		const auto defaultSetMap { &defaultSet<std::unordered_map<CRParameter,unsigned int>,CRParameter,unsigned int> };

		unsigned int call_nb { defaultSetMap(call_number,crparm,0) };
		if (call_nb > static_cast<unsigned int>(parm.getMax(lang)))
			throw iexc(exc::ArgumentNBError,"Too many arguments entered for the required parameter",arg.pos,"Parameter defined here: ",parm.getPositions());


		return parm;
	}

}
