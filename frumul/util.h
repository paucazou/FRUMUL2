#ifndef UTIL_H
#define UTIL_H
// this file contains utilities

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include "bstrlib/bstrwrap.h"
#include "exception.h"

namespace frumul {
	using byte = uint_fast8_t;
	std::string slurp (std::ifstream&);
	bst::str readfile (const bst::str&);
	std::pair<byte,byte> splitShort(int_fast16_t);
	unsigned int negative_index(int index,unsigned int length);
	unsigned int negative_index(int index,unsigned int length,bool check);
	bool is_whitespace (const bst::str&);
	bst::str remove_trailing_whitespaces(const bst::str&);
} // namespace 
#endif
