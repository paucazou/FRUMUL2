#ifndef TAILRESULT_H
#define TAILRESULT_H
#include "symbol.h"
namespace frumul {
	class TailResult {
		/* This class represents the result
		 * of the research inside a tag tail
		 */
		public:
			TailResult();
			TailResult(Symbol&);
			TailResult(const TailResult&);

			void setParameterName(const bst::str&);
			void setPrivilegedArgument(const bst::str&);
			void setSymbol(Symbol&);

			Symbol& getSymbol();
			const bst::str& getParameterName() const;
			const bst::str& getPrivilegedArgument() const;

			bool hasParameterName() const;
			bool hasPrivilegedArgument() const;

		private:
			Symbol* symbol{nullptr};
			bst::str parm_name;
			bst::str priv_arg;
	};
}
#endif

