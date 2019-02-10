#include <cassert>
#include <cwctype>
#include <ios>
#include <iostream>
#include <system_error>
#include "macros.h"
#include "util.h"


namespace frumul {
	std::string slurp (std::ifstream& in) {
		std::stringstream sstr;
		sstr << in.rdbuf();
		return sstr.str();
	}

	FString readfile(const FString& path) {
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

	unsigned int negative_index(int index, unsigned int length) {
		/* Return the positive index of an array (or array-like)
		 * of length 'length'.
		 * If index is not negative, it is returned unchanged
		 */
		if (index >= 0)
			return static_cast<unsigned int>(index);
		return length + static_cast<unsigned int>(index);
	}
	
	unsigned int negative_index(int index, unsigned int length, bool check) {
		/* Throw exception if check is true
		 * and index is over length
		 */
		if (check)
			if ((index < 0 && static_cast<unsigned int>(-index) >= length) || 
			(index >= 0 && static_cast<unsigned int>(index) >= length))
				throw BackException(exc::IndexError);
		return negative_index(index,length);
	}

	bool is_whitespace(const FString& s) {
		/* true if s is a whitespace
		 */
		static const FString unbreakable_space { L'\u00A0' };
		assert(s.uLength() == 1&&"String entered is not a single character");
		const auto raw_char { static_cast<unsigned int>(s.uRawAt(0)) };
		return std::iswspace(raw_char) || s == unbreakable_space;
	}

	FString remove_trailing_whitespaces(const FString& s) {
		/* Remove the trailing whitespace
		 */
		for (int i{s.uLength()-1}; i >= 0; --i) {
			if (!is_whitespace(s.uAt(i))) {
				return s.uRange(0,i);
			}
		}
		return "";
			


		
	}

} // namespace 
