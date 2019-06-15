/* This file
 * is just
 * a example
 * for how to call
 * a function
 * from FRUMUL
 * See bin.uu
 */

#include <vector>
#include "fstring.h"
#include "valvar.h"

extern "C" frumul::ValVar function(const frumul::FString&, const std::vector<frumul::ValVar>&) {
	/* This function does not
	 * use the parameters,
	 * and return a simple text
	 */
	return frumul::FString("This is a simple text returned by an external function");
}
