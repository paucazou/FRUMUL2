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
		public:
			Stdin();
			virtual FString call(const std::vector<ValVar>&, const FString& lang) override;
			virtual ValVar any_call(const std::vector<Arg>&, const FString& lang) override;
#ifdef DEBUG
			virtual void real_type() const override {
				std::cout << "Stdin" << std::endl;
			}
#endif
	};

	class __frumul__out : public Symbol {
		public:
			__frumul__out(std::ostream&,const FString& short_name, const FString& long_name);
			virtual FString call(const std::vector<ValVar>&, const FString& lang) override;
			virtual ValVar any_call(const std::vector<Arg>&, const FString& lang) override;
#ifdef DEBUG
			virtual void real_type() const override {
				std::cout << "__frumul__out" << std::endl;
			}
#endif
		protected:
			std::ostream& stream_;
	};

#if 0
	class File : public Symbol {
		public:
			File();
			virtual FString call(const std::vector<ValVar>&, const FString& lang) override;
			virtual ValVar any_call(const std::vector<Arg>&, const FString& lang) override;
#ifdef DEBUG
			virtual void real_type() const override {
				std::cout << "IO::File" << std::endl;
			}
#endif

	};
#endif

	class IO : public Symbol {
		/* namespace including
		 * all the symbols inside
		 */
		public:
			IO();
		private:
			static __frumul__out out;
			static __frumul__out err;
			static Stdin in;
	};
	extern "C" std::unique_ptr<Symbol> create_symbol();

}

#endif