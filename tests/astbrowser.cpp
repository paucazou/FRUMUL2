#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "tests.h"
#include "../frumul/bstrlib/bstrwrap.h"
/* Browse through the AST returned by parser
 */


std::string slurp (std::ifstream& in) {
	std::stringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

int main(int argc, char* argv[])

{
	std::set_terminate(frumul::terminate);
	if (argc > 1) {
		std::ifstream fileopened (argv[1]);
		bst::str source {slurp(fileopened)};
		std::cout << "Source:\n";
		std::cout << source << "\n\n";
		bst::str filepath {argv[1]};
		frumul::Parser parser {source,filepath};
		// browse ast
		ftest::astBrowser(parser.parse());
		// browse symbols
		ftest::symbolBrowser (parser.getHeaderSymbol());

	}
	else
		std::cerr << "No file entered.\n";

	return 0;
}
