#ifndef IO_STDLIB_FRUMUL_H
#define IO_STDLIB_FRUMUL_H
#include <iostream>
#include <memory>
#include <vector>
#include "fstring.h"
#include "symbol.h"
#include "valvar.h"

namespace frumul {
	
	class Stdin : public Symbol {
		Stdin();
	};

	class __frumul__out : public Symbol {
		public:
			__frumul__out(std::ostream&,const FString& short_name, const FString& long_name);
			virtual FString call(const std::vector<ValVar>&, const FString& lang);
			virtual ValVar any_call(const std::vector<Arg>&, const FString& lang);
		protected:
			std::ostream& stream_;
	};

	class File : public Symbol {
	};

	class IO : public Symbol {
		/* namespace including
		 * all the symbols inside
		 */
		public:
			IO();
	};
	extern "C" std::unique_ptr<Symbol> create_symbol();

}

#endif
