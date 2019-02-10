#ifndef CMDLINE_H
#define CMDLINE_H
#include <initializer_list>
#include <vector>
#include "cxxopts.hpp"
#include "fstring.h"

namespace frumul {
	using String = icu::UnicodeString;
	using ParametersList = std::initializer_list<std::vector<std::string>>;
	static FString __language;
	extern const FString& language;

	cxxopts::Options get_options();
	void manage_args(cxxopts::Options&, int argc, char ** argv);

	bool check_args_compatibility(const cxxopts::Options&, const cxxopts::ParseResult&, const ParametersList);

	template <typename ...T>
		bool check_args_compatibility(const cxxopts::Options& options, const cxxopts::ParseResult& result ,const T... params) {
			return check_args_compatibility(options, result, {params...});
		}
}
#endif
