#include <cassert>
#include "parmqueuer.h"
#include "exception.h"
#include "functions.inl"
#include "macros.h"
#include "vm.h"
#include "vmtypes.h"

namespace frumul {
	ParmQueuer::ParmQueuer (ParmVect& p, const bst::str& nlang) :
	parms{p}, lang{nlang}
	{
		/* Constructor
		 */
		for (size_t i{0}; i < parms.size(); ++i)
			checked.push_back(false);

	}

	Parameter& ParmQueuer::operator() (const bst::str& val, const Position& pos, const bst::str& name) {
		/* This overloaded function
		 * is a simple wrapper.
		 * It creates a temporary Arg
		 * with the parameters entered
		 * but with a void type,
		 * and call operator() (const Arg&)
		 */
		Arg fake_arg {ExprType::VOID,name,val,pos};
		return this->operator() (fake_arg);
	}

	Parameter& ParmQueuer::operator() (const Arg& arg) {
		/* Return the next parameter
		 * matching the arg
		 */
		assert(pos <= parms.size()&&"Pos is too large");
		// get the parameter
		Parameter& parm { select_parm(arg) };
		CRParameter crparm{parm};
		
		// check number
		const auto defaultSetMap { &defaultSet<std::unordered_map<CRParameter,unsigned int>,CRParameter,unsigned int> };

		unsigned int call_nb { defaultSetMap(call_number,crparm,0) };
		if (call_nb > static_cast<unsigned int>(parm.getMax(lang)))
			throw iexc(exc::ArgumentNBError,"Too many arguments entered for the required parameter",arg.pos,"Parameter defined here: ",parm.getPositions());


		return parm;
	}

	Parameter& ParmQueuer::select_parm(const Arg& arg) {
		/* Select a parameter
		 * and return it
		 */
		// with a name
		if (arg.name) {
			for (size_t i{0}; i < parms.size(); ++i) {
				auto& p { parms[i] };
				if (p.getName() == arg.name) {
					// is parameter already checked ?
					if (checked[i])
						throw exc(exc::ArgumentNBError,"Too many arguments were entered",arg.pos);
					// check the parameter if not multiple
					if (p.getMax(lang) == 1)
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
		checked.at(pos) = true;
		return parms.at(pos++);
	}

	bool ParmQueuer::areParametersFilled() const {
		/* true if all parameters have been checked
		 */
		for (const auto& b : checked)
			if (!b)
				return false;
		return true;

	}

	void ParmQueuer::markFinished(const Parameter& parm) {
		/* Mark the parameter parm
		 * as filled
		 * This function must be called with much care.
		 */
		for (size_t i{0}; i < parms.size(); ++i) {
			if (&parms[i] == &parm) { // it is possible this is not a good idea
				checked[i] = true;
				return;
			}
		}
		assert(false&&"Parameter not found");
	}



}
