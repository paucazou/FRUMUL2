/* This file contains many
 * macro utilities
 */

// enum to strings
//https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c

#define GENERATE_ENUM(ENUM) ENUM, // note the comma at the end
#define GENERATE_STRING(STRING) #STRING, // note the comma at the end
// NAME_ENUM is the name of your enum, LIST is the list of items of the enum
#define ENUM(NAME_ENUM, LIST) \
	enum NAME_ENUM { \
		LIST(GENERATE_ENUM) \
	}; \
	const bst::str typeToString (NAME_ENUM t) const { \
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
