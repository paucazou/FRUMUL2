#ifndef STACKVAR_H 
#define STACKVAR_H
/* This file defines the class used
 * in the VM stack
 * the values 
 */
#include <cassert>
#include <functional>
#include <variant>
#include <vector>
#include "valvar.h"

namespace frumul {
	class StackVar {

		public: // constructors
			StackVar(){
				/* empty constructor */
			}
			template <typename T>
				StackVar(const T& elt) : data_ { elt }
			{
			}

			template <typename T>
				StackVar(const T&& elt) : data_ { std::move(elt) }
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
		public: // cast
			operator ValVar () const {
				/* Casts the instance into 
				 * a ValVar if it can do that
				 */
				if (std::holds_alternative<VV::INT>(data_))
					return as<VV::INT>();
				if (std::holds_alternative<VV::BOOL>(data_))
					return as<VV::BOOL>();
				if (std::holds_alternative<VV::STRING>(data_))
					return as<VV::STRING>();
				if (std::holds_alternative<VV::SYMBOL>(data_))
					return as<VV::SYMBOL>();
				if (std::holds_alternative<VV::LIST>(data_))
					return as<VV::LIST>();

				assert(false&&"StackVar can not be cast to a ValVar");
				}
#ifdef DEBUG
			size_t index() const {
				/* return the index of the underlying
				 * variant
				 */
				return data_.index();
			}
#endif

		private:
			std::variant<VV::INT,
				    unsigned int,
				    VV::BOOL,
				    VV::STRING,
				    VV::SYMBOL, 
				    VV::LIST
				    > data_;
	};
	template <>
		StackVar::StackVar(const ValVar&);
}
#endif
