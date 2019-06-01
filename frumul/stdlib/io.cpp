#include <iostream>
#include "exception.h"
#include "io.h"
#include "parameters.h"
#include "util.h"
#include "vm.h"
#include "vmtypes.h"

namespace frumul {
	

	Stdin::Stdin () {
		mark.set(1);
		name.add("i");
		name.add("stdin");
	}

	FString Stdin::call(const std::vector<ValVar>&, const FString&) {
		std::string entered;
		std::getline(std::cin,entered);
		return entered;
	}

	ValVar Stdin::any_call(const std::vector<Arg>&, const FString&) {
		return Stdin::call({},"");
	}

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
		stream_ << args[0].as<VV::STRING>();
		return "";
	}
	ValVar __frumul__out::any_call(const std::vector<Arg>& args, const FString& lang) {
		if (args.size() == 0)
			throw BackException(exc::ArgumentNBError);
		return call(parameters.formatArgs(args,lang),lang);
	}

	File::File() {
		name.add("f");
		name.add("file");
		mark.set(1);

		Parameter access {"access",ExprType::TEXT,{},*this};
		access.setChoices({ValVar('w'),ValVar('r'),ValVar('a')});
		Parameter path {"path",ExprType::TEXT,{},*this };
		//Parameter stream {"stream",ExprType::TEXT,{},*this};

		parameters.push_back(access);
		parameters.push_back(path);
		//parameters.push_back(stream);
	}

	FString File::call(const std::vector<ValVar>& args, const FString&) {
		auto access { args[0].as<VV::STRING>() };
		auto path { args[1].as<VV::STRING>() };
		if (access == "r") {
			try {
				return readfile(path);
			} catch (std::system_error& ) {
				throw BackException(exc::FileError);
			}
		}
		return "";
	}

	ValVar File::any_call(const std::vector<Arg>& args, const FString& lang) {
		return call(parameters.formatArgs(args,lang),lang);
	}

	IO::IO() {
		// adding stdout
		children->addChildReference(out);
		// adding stderr
		children->addChildReference(err);
		// adding stin
		children->addChildReference(in);
		// adding file
		children->addChildReference(file);
	}

	__frumul__out IO::out { std::cout,"o","stdout" };
	__frumul__out IO::err { std::cerr,"e","stderr" };
	Stdin IO::in;
	File IO::file;



	std::unique_ptr<Symbol> create_symbol() {
		return std::make_unique<IO>();
	}
}
