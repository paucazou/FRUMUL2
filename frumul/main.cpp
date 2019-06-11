#include <iostream>
#include "CLI11.hpp"
#include "cmdline.h"
#include "exception.h"
#include "macros.h"

int main (int argc, char ** argv) {
	THINGS_IN_MAIN;
	auto app { frumul::get_app() };
	CLI11_PARSE(*app,argc,argv);
	frumul::manage_args(*app);

	return 0;
}

