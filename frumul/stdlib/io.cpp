#include <iostream>
#include "io.h"
#include "parameters.h"
#include "vm.h"
#include "vmtypes.h"

namespace frumul {
	__frumul__out::__frumul__out(std::ostream& stream, const FString& short_name, const FString& long_name) :
		stream_ { stream }
	{
		mark.set(1);
		name.add(short_name);
		name.add(long_name);
		parameters.push_back(
				Parameter("stream",ExprType::TEXT,{},*this)
				);
	}

	FString __frumul__out::call(const std::vector<ValVar>& args, const FString&) {
		/* Print args on the standard out
		 * and return an empty string
		 */
#pragma message "Add checks ?"
		stream_ << args[0].as<VV::STRING>();
		return "";
	}
	ValVar __frumul__out::any_call(const std::vector<Arg>& args, const FString&) {
		stream_ << args[0].value.as<VV::STRING>();
		return "";
	}

	IO::IO() {
		// adding stdout
		children->addChild(__frumul__out(std::cout,"o","stdout"));
		// adding stderr
		children->addChild(__frumul__out(std::cerr,"e","stderr"));
	}

	std::unique_ptr<Symbol> create_symbol() {
		return std::make_unique<IO>();
	}
}
