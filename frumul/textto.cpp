#include <locale>
#include "textto.h"
namespace frumul {
        template <>
                int text_to<int>(const bst::str& s) {
                        /* Text to Int
                         */
			// checks that all characters are digits
			for (int i{0}; i < s.uLength(); ++i) {
				const auto elt { static_cast<unsigned int>(s.uRawAt(i)) };
				if (!iswdigit(elt))
					throw BackException(exc::TypeError);
			}

                        try {
                                return static_cast<int>(s);
                        } catch (bst::CBStringException& e) {
                                throw BackException(exc::TypeError);
                        }
                }
        template <>
                bool text_to<bool>(const bst::str& s) {
                        /* Text to bool
                         */
                        static const std::map<bst::str,bool> bools {
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
