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
#include "macros.h"

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
			ValVar(){
				/* empty constructor */
			}
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
				T& as() {
					return std::get<T>(data_);
				}
			template <typename T>
				const T& as() const {
					return std::get<T>(data_);
				}

		public: // observers
			template <typename T>
				bool is() const {
					/* true if the underlying value
					 * is of T type
					 */
					return std::holds_alternative<T>(data_);
				}
#ifdef DEBUG
			size_t index() const {
				/* return the index of the underlying
				 * variant
				 */
				return data_.index();
			}

                        FString toString() const {
                            /* Return a representation of the value
                             */
                            if (is<INT>())
                                return FString(as<INT>());
                            if (is<BOOL>())
                                return FString(as<BOOL>() ? "true":"false");
                            if (is<STRING>())
                                return as<STRING>();
                            if (is<SYMBOL>())
                                //return as<SYMBOL>().get().toString();
                                return FString { "Symbol. Representation as ValVar not yet set"};
                            if (is<LIST>()) {
                                FString r{"["};
                                for (const auto& elt : as<LIST>())
                                    r += elt.toString();
                                r+="]";
                                return r;
                            }
                            return "String"; // -Wreturn-type

                        }
                        STDOUT(ValVar)
#endif

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
