#ifndef TESTS_H
#define TESTS_H
#include "../frumul/parser.h"
namespace ftest {
	extern void astBrowser(const frumul::Node& parent);
	extern bool symbolBrowser(const frumul::Symbol& s);
}

#endif
