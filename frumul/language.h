#ifndef LANGUAGE_H
#define LANGUAGE_H
// This header only stores a reference to the language used
// by the user when calling frumul
// Every file needing to use language should include it

#include "fstring.h"
namespace frumul {
	extern const FString& language;
}
#endif
