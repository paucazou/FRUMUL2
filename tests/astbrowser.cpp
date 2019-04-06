#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "tests.h"
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
	bool must_browse { [&]() {
		if (argc > 2) {
			std::string arg2 = argv[2];
			return arg2 != "--no-browser";
		}
		return true;
	}()};

	if (argc > 1) {
		std::ifstream fileopened (argv[1]);
		frumul::FString source {slurp(fileopened)};
		if (must_browse) { 
			std::cout << "Source:\n";
			std::cout << source << "\n\n";
		}
		frumul::FString filepath {argv[1]};
		frumul::Transpiler transpiler{source,filepath,"every"};
		frumul::Parser& parser{transpiler.getParser()};
		if (must_browse) {
			// browse ast
			ftest::astBrowser(parser.parse());
			// browse symbols
			ftest::symbolBrowser (parser.getHeaderSymbol());
			// print result
			printl("Output:\n=======");
		}
		std::cout << transpiler.getOutput();

	}
	else
		std::cerr << "No file entered.\n";

	return 0;
}
