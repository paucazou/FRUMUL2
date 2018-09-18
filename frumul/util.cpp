#include <ios>
#include <iostream>
#include <system_error>
#include "util.h"


namespace frumul {
	std::string slurp (std::ifstream& in) {
		std::stringstream sstr;
		sstr << in.rdbuf();
		return sstr.str();
	}

	bst::str readfile(const bst::str& path) {
		/* Load and read the full content
		 * of a file
		 */
		std::ifstream fileopened (reinterpret_cast<char*>(path.data));
		if (!fileopened)
			throw std::system_error(std::make_error_code(std::io_errc::stream),"File error");
		return slurp(fileopened);
	}

	std::pair<byte,byte> splitShort(int_fast16_t i) {
		/* Split i in two parts and return it
		 * as a pair of bytes. The first one represents
		 * the most important bytes
		 */
		byte a { static_cast<byte>(i) };
		byte b { static_cast<byte>(i >> 8) };
		return {b,a};
	}

} // namespace 
