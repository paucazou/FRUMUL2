#ifndef FUNCTIONS_INL
#define FUNCTIONS_INL
/* This file contains only
 * function templates
 */
#include <deque>
#include <experimental/any>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include "bstrlib/bstrwrap.h"
#include "exception.h"

namespace E = std::experimental;

namespace std {
	template <typename T>
		bool operator == (const reference_wrapper<T>& first, const reference_wrapper<T>& second) {
			/* Checks that first and second
			 * point to the same object
			 */
			return &first.get() == &second.get();
		}
}

namespace frumul {

	template <typename T>
	bool intersection(const T& first, const T& second) {
		/* Check wether at least one element of first
		 * is in second.
		 */
		for (const auto & elt: first)
			for (const auto & item : second)
				if (elt == item)
					return true;
		return false;
	}

	template <typename ELT, typename ITERABLE>
	bool in (const ELT& elt, const ITERABLE& iterable) {
		/* Checks wether the elt 
		 * is in iterable
		 */
		for (const auto& item : iterable)
			if (elt == item)
				return true;
		return false;
	}

	template <typename T>
	std::unique_ptr<T> uniq_copy(const std::unique_ptr<T>& elt) {
		/* Return a copy of elt if elt has a value
		 * if not, return an empty unique_ptr
		 */
		if (elt)
			return std::make_unique<T>(*elt);

		return std::unique_ptr<T>();
	}

	
	template <typename T>
		bool can_be_cast_to(const bst::str& s) {
			/* Check that s can be converted 
			 * to T
			 */
			std::string ss((const char*)(s.data));
			std::istringstream iss{ss};
			T tmp;
			return (iss >> tmp) && (iss.eof());
		}

	template <typename T,typename U, typename V>
		V defaultValue(T& elt,U key, V def) {
			/* return value matching key
			   if key exists, else def
		   	*/	   
			try {
				return elt.at(key);
			} catch (std::out_of_range& e) {
				return def;
			}
		}

	template <typename T, typename U>
		U defaultValue(T& elt, int index, U def) {
			/* Same as previous one, but with an
			 * index known to be an int
			 */
			return defaultValue<T,int,U>(elt,index,def);
		}

	template <typename T, typename U, typename V>
		V defaultSet(T& elt, U key, V def) {
			/* Similar to defaultValue,
			 * but if key is not found,
			 * set elt.at(key) to def
			 * and return def
			 */
			try {
				return elt.at(key);
			} catch (std::out_of_range& e) {
				elt.insert({key,def});
				return elt.at(key);
			}
		}

	template <typename T>
		T modulus(T x, T m) {
			/* Return the real modulus,
			 * even for negative numbers
			 * T is supposed to be an integer type
			 */
			//https://stackoverflow.com/questions/1082917/mod-of-negative-number-is-melting-my-brain
			return (x % m + m) % m;
		}



	// overloaded functions: from text to T
	template <typename T>
		T text_to(const bst::str& s) {
			return T(s);
		}

	// cast equal
	template <typename T>
		bool cast_equal(const E::any& first, const E::any& second) {
			/* true if first and second, cast to T,
			 * are equal
			 */
			return E::any_cast<T>(first) == E::any_cast<T>(second);
		}
	
	// Random access Stack
	template <typename T, typename Container=std::deque<T>>
		class RandomStack : public std::stack<T,Container> {
			/* Stack wich give access
			 * to random elements
			 */
			public:
				T& operator[](unsigned int i) {
					/* unsafe way to get a T
					 */
					return std::stack<T>::c[i];
				}

				const T& operator[](unsigned int i) const {
					return std::stack<T>::c[i];
				}

				T& at(unsigned int i) {
					/* Raise exception
					 * if out of range
					 */
					return std::stack<T>::c.at(i);
				}

				const T& at(unsigned int i) const {
					return std::stack<T>::c.at(i);
				}

				T& topMin(unsigned int i){
					/* get the element before top
					 * by i span
					 * top = 0
					 */
					return at(std::stack<T>::size()-i-1);
				}

				T pop() {
					/* return last element and delete
					 * it
					 */
					T last{std::stack<T,Container>::top()};
					std::stack<T,Container>::pop();
					return last;
				}

				const T& topMin(unsigned int i) const {
					/* idem in const context
					 */
					return at(std::stack<T>::size()-i-1);
				}

				Container& exposeContainer () {
					/* expose the container
					 */
					return std::stack<T>::c;
				}

		};

	template <typename T>
		using rstack = RandomStack<T>;

}//namespace
#endif
