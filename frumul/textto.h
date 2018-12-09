#ifndef TEXTTO_H
#define TEXTTO_H
#include "functions.inl"
namespace frumul {
	template<>
		bool text_to<bool>(const bst::str& s);
	template<>
		int text_to<int>(const bst::str& s);
}
#endif
