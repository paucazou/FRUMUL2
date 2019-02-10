#include <cassert>
#include "tailresult.h"

namespace frumul {

	TailResult::TailResult() 
	{
	}

	TailResult::TailResult(Symbol& s) :
		symbol{&s}
	{
	}

	TailResult::TailResult(const TailResult& tr) :
		symbol{tr.symbol}, parm_name{tr.parm_name}, priv_arg{tr.priv_arg}
	{
	}

	void TailResult::setParameterName(const FString& s) {
		assert(!parm_name&&"Parameter name already set");
		parm_name = s;
	}

	void TailResult::setPrivilegedArgument(const FString& s) {
		assert(!priv_arg&&"Privileged argument already set");
		priv_arg = s;
	}

	void TailResult::setSymbol(Symbol& s) {
		assert(!symbol&&"Symbol already set");
		symbol = &s;
	}

	Symbol& TailResult::getSymbol() {
		assert(symbol&&"Symbol not set");
		return *symbol;
	}

	const FString& TailResult::getParameterName() const {
		assert(parm_name&&"No parm name");
		return parm_name;
	}

	const FString& TailResult::getPrivilegedArgument() const {
		assert(priv_arg&&"No privileged argument");
		return priv_arg;
	}

	bool TailResult::hasParameterName() const {
		/* true if the instance has collected a parameter
		 * name
		 */
		return parm_name;
	}

	bool TailResult::hasPrivilegedArgument() const {
		/* true if the instance has collected 
		 * a privileged argument
		 */
		return priv_arg;
	}
}

