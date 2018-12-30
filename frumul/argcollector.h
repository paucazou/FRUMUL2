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
			~ArgCollector();
			// setters
			void collect(const Node&);
			// getters
			const std::vector<E::any>& getArgs()const;
			bool expectsArgs() const;
			bool isLastMultipleParmFilled() const;
			void finishMultipleArgs();
			// overloads
			void operator << (const Node&);
			void operator >> (std::vector<E::any>&)const;
		private:
			Symbol& symbol;
			const Mark& mark;
			Parameters& parameters;
			ParmQueuer queue;
			const bst::str& lang;
			std::vector<E::any> args;

			// multiple parameter
			std::vector<E::any> current_multiple_args;
			std::vector<Position> current_args_pos;
			Parameter* multiple_parm{nullptr};
			// functions
			E::any format_arg(const Parameter&, const Node&);
			void _collect(const Node&,Parameter&);
			void _start_multiple_args(Parameter&);
			void _finish_arg(const Node&,const E::any&,Parameter&);

	};
}
#endif
