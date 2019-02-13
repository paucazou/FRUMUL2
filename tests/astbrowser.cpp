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
	if (argc > 1) {
		std::ifstream fileopened (argv[1]);
		frumul::FString source {slurp(fileopened)};
		std::cout << "Source:\n";
		std::cout << source << "\n\n";
		frumul::FString filepath {argv[1]};
		frumul::Transpiler transpiler{source,filepath,"every"};
		frumul::Parser& parser{transpiler.getParser()};
		// browse ast
		ftest::astBrowser(parser.parse());
		// browse symbols
		ftest::symbolBrowser (parser.getHeaderSymbol());
		// print result
		printl("Output:\n=======");
		std::cout << transpiler.getOutput();

	}
	else
		std::cerr << "No file entered.\n";

	return 0;
}
