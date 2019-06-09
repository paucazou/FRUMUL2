#include <cassert>
#include "textto.h"

using namespace frumul;

template <typename E, typename T>
	bool catcher (const T& lambda) {
		/* This function expects to catch
		 * E
		 */
			try {
				lambda();
			} catch (const E&) {
				return true;
			}
			return false;
		}


void test_text_to_int() {
	// positive
	assert(text_to<int>("1") == 1);
	
	// negative
	assert(text_to<int>("-1") == -1);
	// errors
	catcher<BackException>([]() {text_to<int>("a");});
}

int main () {
	test_text_to_int();
	std::cout << "Everything is fine.\n";
	return 0;
}
	

