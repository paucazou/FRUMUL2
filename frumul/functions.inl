#ifndef FUNCTIONS_INL
#define FUNCTIONS_INL
/* This file contains only
 * function templates
 */
#include <memory>
#include <stack>

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

	template<typename T>
		class RandomStack : public std::stack<T> {
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
				const T& topMin(unsigned int i) const {
					/* idem in const context
					 */
					return at(std::stack<T>::size()-i-1);
				}
		};

	template <typename T>
		using rstack = RandomStack<T>;

}//namespace
#endif
