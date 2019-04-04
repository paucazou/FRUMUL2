#ifndef UTIL_H
#define UTIL_H
// this file contains utilities

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include "fstring.h"
#include "exception.h"

namespace fs = std::filesystem;

namespace frumul {
	using byte = uint_fast8_t;
	std::string slurp (std::ifstream&);
	FString readfile (const FString&);
	std::pair<byte,byte> splitShort(int_fast16_t);
	unsigned int negative_index(int index,unsigned int length);
	unsigned int negative_index(int index,unsigned int length,bool check);
	bool is_whitespace (const FString&);
	FString remove_trailing_whitespaces(const FString&);
	fs::path get_path(const fs::path&,const fs::path&);
} // namespace 
#endif
