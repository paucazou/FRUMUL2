#ifndef ARG_COLLECTOR_H
#define ARG_COLLECTOR_H
#include <experimental/any>
#include <vector>
#include "mark.h"
#include "parameters.h"
#include "parmqueuer.h"
#include "symbol.h"

namespace E = std::experimental;

namespace frumul {

	class ArgCollector {
		/* Class that collects
		 * textual arguments,
		 * checks them and return them to a call
		 */
		public:
			ArgCollector(Symbol&,const bst::str&);
			// setters
			void collect(const Node&);
			// getters
			const std::vector<E::any>& getArgs()const;
			bool expectsArgs() const;
			// overloads
			void operator << (const Node&);
			void operator >> (std::vector<E::any>&)const;
		private:
			Symbol& symbol;
			const Mark& mark;
			Parameters& parameters;
			ParmQueuer queue;
			std::vector<E::any> args;
			// functions
			E::any format_arg(const Parameter&, const Node&);
			void _collect(const Node&,const Parameter&);

	};
}
#endif
