#include <cassert>
#include <cstdlib>
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
		//std::ifstream fileopened { reinterpret_cast<const char*>(path.data()) };
		std::ifstream fileopened {path.toUTF8String<std::string>().data() };
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
		assert(s.length() == 1&&"String entered is not a single character");
		const auto raw_char { static_cast<unsigned int>(s.rawAt(0)) };
		return std::iswspace(raw_char) || s == unbreakable_space;
	}

	FString remove_trailing_whitespaces(const FString& s) {
		/* Remove the trailing whitespace
		 */
		for (int i{s.length()-1}; i >= 0; --i) {
			if (!is_whitespace(s[i])) {
				return s.extract(0,i);
			}
		}
		return "";
	}
	fs::path operator + (const fs::path& first, const char* ext) {
		/* Adds an extension to first and
		 * return it as a new fs::path
		 */
		fs::path new_path{first};
		return new_path+=ext;
	}
	fs::path operator + (const fs::path& first, const fs::path& second) {
		/* Append second to first with a '/' between them
		 * and return a new path
		 */
		fs::path third { first };
		return third /= second;
	}

	fs::path get_path(const fs::path& file, const fs::path& parent) {
		/* file is supposed to be the name of a header
		 * this function tries to find the correct header
		 * with the following order:
		 * 	1 is it a user defined text header? (.h)
		 * 	2 is it a user defined binary header? (.hb)
		 * 	3 is it a standard text header?
		 * 	4 is it a standard binary header?
		 * If none of these is found, throw
		 * a BackException
		 */
		fs::path stdlib { std::getenv("FRUMUL_STDLIB") };
		assert(! stdlib.empty() && "Stdlib path is empty");

		fs::path real_file;
		if (!fs::exists(real_file = parent + file + ".h") &&
		    !fs::exists(real_file = parent + file + ".hb") &&
		    !fs::exists(real_file = stdlib + file + ".h") &&
		    !fs::exists(real_file = stdlib + file + ".hb")
		   )
			throw BackException(exc::FileError);

		return real_file;
	}

	fs::path get_real_path(const FString& base, const FString& second) {
		/* Get a path by appending base to second.
		 * base is the calling file, second the called
		 */
		fs::path calling { base.toUTF8String<std::string>() };
		fs::path parent { calling.parent_path() };
		fs::path called { second.toUTF8String<std::string>() };
		return parent + called;
	}

} // namespace 
