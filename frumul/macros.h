/* This file contains many
 * macro utilities
 */
#include <cstdint>
#include <iostream>

/*****************
*  enum to strings
*****************/
//https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c

#define GENERATE_ENUM(ENUM) ENUM, // note the comma at the end
#define GENERATE_STRING(STRING) #STRING, // note the comma at the end
// NAME_ENUM is the name of your enum, LIST is the list of items of the enum
#define ENUM(NAME_ENUM, LIST) \
	enum NAME_ENUM { \
		LIST(GENERATE_ENUM) \
	}; \
	static const bst::str typeToString (NAME_ENUM t) { \
		static const bst::str enumstring[] = { \
			LIST(GENERATE_STRING) \
		}; \
		return enumstring[t]; \
	}

/* Usage:
 * Define your enum like the following example:
 * #define FOREACH_FRUIT(F)\ // note: no space between parentheses and the name
 * 	F(APPLE)\
 * 	F(ORANGE)\
 * 	etc.
 *
 * ENUM(FRUIT_ENUM,FOREACH_FRUIT)
 */

/*******************************
// things that must go into main
*******************************/
#define THINGS_IN_MAIN \
	std::setlocale(LC_ALL,std::locale("").name().data());\
	std::set_terminate (frumul::terminate)

/******************************
 * Print a class to stdout.
 * This class must have toString
 * member function
 * T is a typename
 ******************************/

#define STDOUT(T) \
	friend std::ostream& operator<< (std::ostream& out, const T& elt) {\
		out << elt.toString();\
		return out;\
	}

/***********************
 * Definition of aliases
 * *********************/
namespace std {
	using byte = uint_fast8_t;
}

#ifdef DEBUG
#define printl(elt) std::cout << ( elt ) << std::endl
#else
#define printl(elt) 
#endif
