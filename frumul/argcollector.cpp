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
		queue{symbol.getParameters().getList(), l},
		lang{l}
	{
	}

	void ArgCollector::collect(const Node& n) {
		/* Wrapper of _collect
		 * It manages the name of the arg
		 * (if there is one)
		 */
		switch (n.type()) {
			case Node::TEXTUAL_ARGUMENT:
				_collect(n,queue(n.getValue(),n.getPosition()));
				break;
			case Node::NAMED_ARG:
				_collect(n.get("arg_value"),
					queue(n.get("arg_value").getValue(),n.getPosition())
					);
				break;
			default:
				assert(false&&"Node not recognized");

		};
	}


	void ArgCollector::_collect (const Node& n, Parameter& parm) {
		// format the arg to match the type
		// and checks it matches
		E::any value { format_arg(parm,n) };
		// check choice TODO
		if (!parm.choiceMatch(value,lang)){
				throw iexc(exc::ValueError,"Value entered does not match the choices set. Value entered: ",n.getPosition(),"Choices: ",parm.getChoices().getPosition());
		}	
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

