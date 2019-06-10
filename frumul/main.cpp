#include <iostream>
#include "CLI11.hpp"
#include "cmdline.h"

int main (int argc, char ** argv) {
	auto app { frumul::get_app() };
	CLI11_PARSE(*app,argc,argv);
	frumul::manage_args(*app);

	return 0;
}

