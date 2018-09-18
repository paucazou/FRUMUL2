#ifndef UTIL_H
#define UTIL_H
// this file contains utilities

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include "bstrlib/bstrwrap.h"

namespace frumul {
	using byte = uint_fast8_t;
	std::string slurp (std::ifstream&);
	bst::str readfile (const bst::str&);
	std::pair<byte,byte> splitShort(int_fast16_t);
} // namespace 
#endif
