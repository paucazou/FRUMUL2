#include <cassert>
#include "argcollector.h"
#include "textto.h"
#include "node.h"
#include "util.h"

namespace frumul {
	ArgCollector::ArgCollector(Symbol& s, const bst::str& l):
		symbol{s},
		mark{symbol.getMark()},
		parameters{symbol.getParameters()},
		queue{symbol.getParameters().getList(), l}
	{
	}

	void ArgCollector::collect (const Node& n) {
		assert(n.type() == Node::TEXTUAL_ARGUMENT&&"Node not recognized");
		// get matching parameter
		const Parameter& parm { queue(n.getValue(),n.getPosition()) };
		// format the arg to match the type
		// and checks it matches
		E::any value { format_arg(parm,n) };
		// check choice TODO
		// append to args
		args.push_back(value);
	}

	const std::vector<E::any>& ArgCollector::getArgs() const {
		return args;
	}

	bool ArgCollector::expectsArgs() const {
		/* true if new arguments are expected
		 */
#pragma message "do not forget mark"
		return !queue.areParametersFilled();
	}

	void ArgCollector::operator << (const Node& n) {
		collect(n);
	}

	void ArgCollector::operator >> (std::vector<E::any>& arg_list) const {
		arg_list = getArgs();
	}

	E::any ArgCollector::format_arg(const Parameter& parm,const Node& n) {
		/* Try to cast the value of the node
		 * into the type expected
		 */

		const ExprType type { parm.getType()};
		try {
			if (type == ExprType::INT) {
				const bst::str v { remove_trailing_whitespaces(n.getValue()) };
				return text_to<int>(v);
				}
			if (type == ExprType::BOOL) {
				const bst::str v { remove_trailing_whitespaces(n.getValue()) };
				return text_to<bool>(v);
				}
			if (type != ExprType::TEXT)
				throw BackException(exc::TypeError);

			return n.getValue();

		} catch (const BackException& e) {
			throw iexc(exc::TypeError,"Argument entered does not match the type of the parameter. Argument: ",n.getPosition(),"Parameter: ",parm.getPositions());
		}
	}
}

