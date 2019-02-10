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

			void setParameterName(const FString&);
			void setPrivilegedArgument(const FString&);
			void setSymbol(Symbol&);

			Symbol& getSymbol();
			const FString& getParameterName() const;
			const FString& getPrivilegedArgument() const;

			bool hasParameterName() const;
			bool hasPrivilegedArgument() const;

		private:
			Symbol* symbol{nullptr};
			FString parm_name;
			FString priv_arg;
	};
}
#endif

