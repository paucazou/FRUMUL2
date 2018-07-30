#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "../frumul/parser.h"
#include "../frumul/bstrlib/bstrwrap.h"
/* Browse through the AST returned by parser
 */

void browser (const frumul::Node& parent) {
	/* Takes a node
	 * print it and let the user
	 * choose a child
	 * To select the parent of the parent,
	 * please enter 'parent'
	 */

back: // there is a goto at the end of the function
	std::cout << parent;
	if (parent.getValue())
		std::cout << "VALUE: " << parent.getValue() << "\n";
	std::cout << "Enter a name of a child, or 'parent' for the parent: ";
	bst::str answer;
	std::cin >> answer;
	if (answer == "parent")
		return;
	
	try {
		if (parent.areChildrenNamed())
			browser(parent.get(answer));
		else 
			browser(parent.get(static_cast<int>(answer)));
	}
	catch (std::out_of_range) {
		std::cerr << "No child of that name\n";
	}
	goto back; // yes, it's bad
}

void symbolBrowser(const frumul::Symbol& s) {
	/* Takes a symbol and prints
	 * the major informations
	 * about it
	 */
back:
	std::cout << s;
	std::cout << "Enter the name of a child, '--parent','--alias': ";
	bst::str answer;
	std::cin >> answer;
	if (answer == "--parent")
		return;
	if (answer == "--alias") {
		assert(s.hasAlias()&& "Symbol has no alias");
		symbolBrowser(s.getAlias().getVal());
	}
	else
		symbolBrowser(s.getChildren().getChild(answer));

	goto back;


}

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
		browser (parser.parse());
		// browse symbols
		symbolBrowser (parser.getHeaderSymbol());

	}
	else
		std::cerr << "No file entered.\n";

	return 0;
}
