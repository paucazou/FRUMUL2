/* This file contains some tests
 * for the ValVar class
 * We use here the typedefs declared in the ValVar class
 * in order to avoid modifications if the underlying implementation
 * changes.
 */
#include <cassert>
#include "symbol.h"
#include "valvar.h"


using namespace frumul;

bool test_constructor() {
	// with int
	ValVar v_int { 1 };
	// with bool
	ValVar v_bool { true };
	// with string
	ValVar v_string { "a string" };
	// Symbol
	Symbol sym;
	ValVar v_symbol { VV::SYMBOL(sym) };
	// list 
	VV::LIST list { 45, true, "A string" };
	// copy
	ValVar v_copy { v_string };
	// move
	ValVar v_move { std::move(v_string) };
	
	return true;
}

bool test_get_underlying_value() {
	// int
	VV::INT i { 200 };
	ValVar vvi { i };
	assert(vvi.as<VV::INT>() == i);
	// bool
	VV::BOOL b { false };
	ValVar vvb { b };
	assert(vvb.as<VV::BOOL>() == b);

	// string
	VV::STRING s { "A string" };
	ValVar vvs { s };
	assert(vvs.as<VV::STRING>() == s);

	// symbol
	Symbol sym;
	VV::SYMBOL sym_ { sym };
	ValVar vsym { sym_ };
	auto adress = &vsym.as<VV::SYMBOL>().get();
	assert (adress == &sym);

	return true;
}

void print_sizes() {
	printl("INT size:");
	printl(sizeof(VV::INT));
	printl("BOOL size:");
	printl(sizeof(VV::BOOL));
	printl("STRING size:");
	printl(sizeof(VV::STRING));
	printl("LIST size:");
	printl(sizeof(VV::LIST));
	printl("SYMBOL size:");
	printl(sizeof(VV::SYMBOL));
	printl("ValVar size:");
	printl(sizeof(ValVar));
}

int main() {
	test_constructor();
	test_get_underlying_value();
	print_sizes();
	return 0;
}


