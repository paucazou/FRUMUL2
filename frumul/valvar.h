#ifndef VALVAR_H
#define VALVAR_H
/* This file defines the class used
 * inside the values to carry
 * the variables
 */
#include <functional>
#include <variant>
#include <vector>
#include "fstring.h"

namespace frumul {
	class Symbol;

	class ValVar {
		public: // typedefs
			using INT = int;
			using BOOL = bool;
			using STRING = FString;
			using SYMBOL = std::reference_wrapper<Symbol>;
			using LIST = std::vector<ValVar>;

		public: // constructors
			template <typename T>
				ValVar(const T& elt) : data_ { elt }
			{
			}

			template <typename T>
				ValVar(const T&& elt) : data_ { std::move(elt) }
			{
			}

		public: // getters
			template <typename T>
				T as() {
					return std::get<T>(data_);
				}

		private:
			std::variant<INT,
				    BOOL,
				    STRING,
				    SYMBOL, 
				    LIST
				    > data_;
	};
	using VV = ValVar;
}
#endif
