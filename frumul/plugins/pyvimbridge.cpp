#include <string>
#include <vector>
#include "fstring.h"
#include "parser.h"


extern "C" const char* get_tags(const char* buffer, const char* name) {
	/* Takes a buffer,
	 * and return a pointer to a list of tags
	 */
	static std::string names;
	names = "";

	const frumul::FString source { buffer };
	const frumul::FString path { name };

	try {
		frumul::Parser parser(source,path);
		parser.parseHeaderOnly();

		auto fstring_names = parser.getHeaderSymbol().getChildrenNames();
		for (const auto& n : fstring_names) {
			names += n.toUTF8String<std::string>() + " ";
		}
	} catch (const frumul::exc&) {
		return names.data();
	}

	return names.data();

}

