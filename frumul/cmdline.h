#ifndef CMDLINE_H
#define CMDLINE_H
#include <initializer_list>
#include <memory>
#include <vector>
#include "fstring.h"
#include "CLI11.hpp"

namespace frumul {
	//using ParametersList = std::initializer_list<std::vector<std::string>>;

	static FString __language{"every"};// default value. Can be changed by the cli parser
	extern const FString& language;

	std::shared_ptr<CLI::App> get_app();
	void manage_args(CLI::App&);

	//bool check_args_compatibility(const cxxopts::ParseResult&, const ParametersList);

	/*template <typename ...T>
		bool check_args_compatibility(const cxxopts::ParseResult& result ,const T... params) {
			return check_args_compatibility(result, {params...});
		}
		*/
}
#endif
