#ifndef CMDLINE_H
#define CMDLINE_H
#include <initializer_list>
#include <vector>
#include "CLI11.hpp"
#include "fstring.h"

namespace frumul {
	using ParametersList = std::initializer_list<std::vector<std::string>>;

	static FString __language;
	extern const FString& language;

	cxxopts::Options get_options();
	void manage_args(cxxopts::Options&, int argc, char ** argv);

	bool check_args_compatibility(const cxxopts::ParseResult&, const ParametersList);

	template <typename ...T>
		bool check_args_compatibility(const cxxopts::ParseResult& result ,const T... params) {
			return check_args_compatibility(result, {params...});
		}
}
#endif
