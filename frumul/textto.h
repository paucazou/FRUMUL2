#ifndef TEXTTO_H
#define TEXTTO_H
#include "functions.inl"
namespace frumul {
	template<>
		bool text_to<bool>(const FString& s);
	template<>
		int text_to<int>(const FString& s);
}
#endif
