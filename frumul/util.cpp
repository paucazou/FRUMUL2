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

} // namespace 
