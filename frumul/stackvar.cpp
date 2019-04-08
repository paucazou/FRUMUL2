#include "stackvar.h"

namespace frumul {
	template <>
		StackVar::StackVar(const ValVar& elt) 
		{
#define copy_if_type(T) \
		if (elt.is< T >()) \
			data_ = elt.as< T >(); \
		else

			copy_if_type(VV::INT)
			copy_if_type(VV::STRING)
			copy_if_type(VV::SYMBOL)
			copy_if_type(VV::BOOL)
			copy_if_type(VV::LIST)
			assert(false&&"Type not recognized");
#undef copy_if_type
		}

}
