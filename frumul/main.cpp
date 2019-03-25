#include <iostream>
#include "cxxopts.hpp"
#include "cmdline.h"

int main (int argc, char ** argv) {
	auto options { frumul::get_options() };
	frumul::manage_args(options,argc,argv);

	return 0;
}

