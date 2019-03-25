#ifndef ARG_COLLECTOR_H
#define ARG_COLLECTOR_H
#include <any>
#include <vector>
#include "mark.h"
#include "parameters.h"
#include "parmqueuer.h"
#include "symbol.h"



namespace frumul {

	class ArgCollector {
		/* Class that collects
		 * textual arguments,
		 * checks them and return them to a call
		 */
		public:
			ArgCollector(Symbol&,const FString&);
			~ArgCollector();
			// setters
			void collect(const Node&);
			// getters
			const std::vector<std::any>& getArgs()const;
			bool expectsArgs() const;
			bool isLastMultipleParmFilled() const;
			bool isCurrentParmMultiple() const;
			void finishMultipleArgs();
			void finishMultipleArgsAfterLastArg();
			void flagNextArgAsNamed(bool);
			void fillDefaultArgs();
			// overloads
			void operator << (const Node&);
			void operator >> (std::vector<std::any>&)const;
		private:
			Symbol& symbol;
			const Mark& mark;
			Parameters& parameters;
			ParmQueuer queue;
			const FString& lang;
			std::vector<std::any> args;
			bool must_finish_mul_parm{false};
			bool is_next_arg_named{false};

			// multiple parameter
			std::vector<std::any> current_multiple_args;
			std::vector<Position> current_args_pos;
			Parameter* multiple_parm{nullptr};
			// functions
			std::any format_arg(const Parameter&, const Node&);
			void _collect(const Node&,Parameter&);
			void _start_multiple_args(Parameter&);
			void _finish_arg(const Node&,const std::any&,Parameter&);

	};
}
#endif
