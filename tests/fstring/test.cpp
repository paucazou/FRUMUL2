#include "fstring.h"
#include <cassert>

using namespace frumul;

constexpr char   str[] {"ù@à κόσμε блф"};

bool test_constructors() {
	// empty
	FString empty;
	assert(!empty);

	// std::string
	std::string s {str};
	FString std_string{s};
	assert(std_string == s.data());

	// const char *
	const char * chars { str};
	FString from_chars {chars};
	assert(from_chars == str);

	//const char
	char x { 'o' };
	FString from_char {x};
	assert(from_char.length() == 1);
	assert(from_char == x);

	// unsigned int
	unsigned int ui {10};
	FString from_ui { ui };
	assert(from_ui == "10");

	// int
	int i { -5 };
	FString from_i { i };
	assert(from_i == "-5");

	return true;
}

int main () {
	return 0;
}
