#include <iostream>
#include <string>
#include "fstring.h"
#include "exception.h"
#include "io.h"
#include "mark.h"
#include "parameters.h"
#include "util.h"
#include "vm.h"
#include "vmtypes.h"

namespace frumul {
	namespace __io__frumul {
		const FString pseudo_file = ""
			"mark «1» lang «every» namespace «i{stdin} o{stdout} e{stderr} f{file}»\n"
			"	stdin 	: «{stdin value}»\n"
			"	stdout 	: arg «stream,text» «{stdout value}»\n"
			"	stderr	: arg «stream,text» «{stderr value}»\n"
			"	file	: arg «access,text»,[«w»,«r»,«a»]¦\n"
			"			path,text¦\n"
			"			stream : «»,text,<=1»\n"
			"			«{file value}»";
		const FString pseudo_path = "io";
		const Position mark_pos = Position(0,7,__io__frumul::pseudo_path,__io__frumul::pseudo_file);

		Position generate_pos(int x, int y) {
			return Position(x,y,pseudo_path,pseudo_file);
		}
	}
	

	Stdin::Stdin () {
		mark.set(1);
		mark.addPos(__io__frumul::mark_pos);

		name.add("i",Position(33,33,__io__frumul::pseudo_path,__io__frumul::pseudo_file));
		name.add("stdin",Position(35,39,__io__frumul::pseudo_path,__io__frumul::pseudo_file));
	}

	FString Stdin::call(const std::vector<ValVar>&, const FString&) {
		std::string entered;
		std::getline(std::cin,entered);
		return entered;
	}

	ValVar Stdin::any_call(const std::vector<Arg>&, const FString&) {
		return Stdin::call({},"");
	}

	__frumul__out::__frumul__out(std::ostream& stream, const FString& short_name, const FString& long_name, const Position& spos, const Position& lpos,const Position& parmpos) :
		stream_ { stream }
	{
		mark.set(1);
		mark.addPos(__io__frumul::mark_pos);

		name.add(short_name,spos);
		name.add(long_name,lpos);
		parameters.push_back(
				Parameter("stream",ExprType::TEXT,{parmpos},*this)
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
		name.add("f",Position(62,62,__io__frumul::pseudo_path,__io__frumul::pseudo_file));
		name.add("file",Position(64,67,__io__frumul::pseudo_path,__io__frumul::pseudo_file));
		mark.set(1);
		mark.addPos(__io__frumul::mark_pos);
		
		// parameters positions
		Position access_pos{200,226,__io__frumul::pseudo_path,__io__frumul::pseudo_file};
		Position path_pos{232,240,__io__frumul::pseudo_path,__io__frumul::pseudo_file};
		Position stream_pos{246,265,__io__frumul::pseudo_path,__io__frumul::pseudo_file};

		Parameter access {"access",ExprType::TEXT,{access_pos},*this};
		//access.setChoices({ValVar("w"_FS),ValVar("r"_FS),ValVar("a"_FS)});
		access.setChoices({FString('w'),FString('r'),FString('a')});
		Parameter path {"path",ExprType::TEXT,{path_pos},*this };
		Parameter stream {"stream",ExprType::TEXT,{stream_pos},*this};
		stream.setMinMax(0,1);
		stream.setDefault("");

		parameters.push_back(access);
		parameters.push_back(path);
		parameters.push_back(stream);
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
		auto mode { access == "w" ? std::ios::out : std::ios::app };
		auto text { args[2].as<VV::STRING>() };
		try {
			std::ofstream stream (path.toUTF8String<std::string>().data(), mode);
			stream << text;
		} catch (std::system_error&) {
			throw BackException(exc::FileError);
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

	__frumul__out IO::out { std::cout,"o","stdout",Position(42,42,__io__frumul::pseudo_path,__io__frumul::pseudo_file), Position(44,49,__io__frumul::pseudo_path,__io__frumul::pseudo_file),__io__frumul::generate_pos(112,124) };
	__frumul__out IO::err { std::cerr,"e","stderr", Position(52,52,__io__frumul::pseudo_path,__io__frumul::pseudo_file), Position(54,59, __io__frumul::pseudo_path,__io__frumul::pseudo_file),__io__frumul::generate_pos(157,169)};
	Stdin IO::in;
	File IO::file;



	std::unique_ptr<Symbol> create_symbol() {
		return std::make_unique<IO>();
	}
}
