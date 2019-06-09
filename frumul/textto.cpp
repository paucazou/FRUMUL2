#include <locale>
#include "textto.h"
namespace frumul {
        template <>
                int text_to<int>(const FString& s) {
                        /* Text to Int
			 * Only acceptable:
			 * 	- pure digits,
			 * 	- digits with '-' before
			 * 	- no spaces allowed
                         */
			// checks that all characters are digits
			// except the first one, which can be a '-'
			for (int i{0}; i < s.length(); ++i) {
				if (i == 0 && s[i] == "-")
					continue;
				const auto elt { static_cast<unsigned int>(s.rawAt(i)) };
				if (!iswdigit(elt))
					throw BackException(exc::TypeError);
			}

                        try {
                                return static_cast<int>(s);
                        } catch (FStringException& e) {
                                throw BackException(exc::TypeError);
                        }
                }
        template <>
                bool text_to<bool>(const FString& s) {
                        /* Text to bool
                         */
                        static const std::map<FString,bool> bools {
                                {"false",false},
                                {"true",true}
                        };
                        try {
                                return bools.at(s);
                        } catch (std::out_of_range& e) {
                                throw BackException(exc::TypeError);
                        }
                }

}
