#include <cassert>
#include "argcollector.h"
#include "textto.h"
#include "node.h"
#include "util.h"

namespace frumul {
	ArgCollector::ArgCollector(Symbol& s, const FString& l):
		symbol{s},
		mark{symbol.getMark()},
		parameters{symbol.getParameters()},
		queue{symbol.getParameters().getList(), l},
		lang{l}
	{
		args.resize(parameters.size());
	}

	ArgCollector::~ArgCollector() {
		/* This destructor does nothing,
		 * except check that all is finished
		 */
		assert(!multiple_parm&&"A Parameter is still in use");
		assert(current_args_pos.size() == 0 && "Please clear the Pos vector");
		assert(current_multiple_args.size() == 0 && "Please clear the arguments vector");
	}

	void ArgCollector::collect(const Node& n) {
		/* Wrapper of _collect
		 * It manages the name of the arg
		 * (if there is one)
		 */
		switch (n.type()) {
			case Node::TEXTUAL_ARGUMENT:
				if (multiple_parm) {
					// finish a multiple parameter if necessary
					finishMultipleArgs();
				}
				_collect(n,queue(n.getValue(),n.getPosition()));
				break;
			case Node::NAMED_ARG:
				_collect(n.get("arg_value"),
					queue(n.get("arg_value").getValue(),n.getPosition(),n.getValue())
					);
				break;
			default:
				assert(false&&"Node not recognized");

		};
		// finish multiple arg ?
		if (must_finish_mul_parm) {
			finishMultipleArgs();
			must_finish_mul_parm = false;
		}
	}

	void ArgCollector::finishMultipleArgs() {
		/* Checks that all the args were collected
		 * and put them in the main list
		 */
		// checks
		int nb { static_cast<int>(current_multiple_args.size()) };
		if (multiple_parm->getMin(lang) > nb && nb > multiple_parm->getMax(lang))
			throw iexc(exc::ArgumentNBError,
				FString("The number of arguments received does not match the number required: ") + nb + " argument(s) entered. Expected minimum: " + multiple_parm->getMin(lang) + ". Maximum: " + multiple_parm->getMax(lang) + "\nArgs defined here:",
				current_args_pos,
				"Parameter defined here: ",
				multiple_parm->getPositions());

		// put the args in the main list
		//args.push_back(current_multiple_args);
		args.at(static_cast<size_t>(multiple_parm->getIndex())) = current_multiple_args;
		// clean-up
		queue.markFinished(*multiple_parm);
		multiple_parm = nullptr;
		current_multiple_args.clear();
		current_args_pos.clear();
	}


	void ArgCollector::_collect (const Node& n, Parameter& parm) {
		// format the arg to match the type
		// and checks it matches
		ValVar value { format_arg(parm,n) };
		
		if (multiple_parm && &parm != multiple_parm)
			finishMultipleArgs();

		if (parm.getMax(lang) != 1) {
			if (!multiple_parm)
				_start_multiple_args(parm);
			current_args_pos.push_back(n.getPosition());
			current_multiple_args.push_back(value);
		}

		else
			_finish_arg(n,value,parm);
	}

	void ArgCollector::_start_multiple_args(Parameter& parm) {
		/* Start the collect of the args of a multiple parameter
		 * If another parm has been in use before, please clean-up
		 * before start this function
		 */
		assert(!multiple_parm&&"A Parameter is still in use");
		assert(current_args_pos.size() == 0 && "Please clear the Pos vector");
		assert(current_multiple_args.size() == 0 && "Please clear the arguments vector");

		multiple_parm = &parm;
	}


	void ArgCollector::_finish_arg(const Node& n,const ValVar& value, Parameter& parm) {
		/* Finish to enter the value
		 * in the args
		 */
		// check choice 
		if (!parm.choiceMatch(value,lang)){
				throw iexc(exc::ValueError,"Value entered does not match the choices set. Value entered: ",n.getPosition(),"Choices: ",parm.getChoices().getPosition());
		}	
		// append to args
		//args.push_back(value);
		args.at(static_cast<size_t>(parm.getIndex())) = value;
	}

	const std::vector<ValVar>& ArgCollector::getArgs() const {
		return args;
	}

	bool ArgCollector::expectsArgs() const {
		/* true if new arguments are expected
		 */
		return !queue.areNonDefaultParametersFilled() ||
			(is_next_arg_named && queue.hasUnfilledDefault());
	}

	bool ArgCollector::isLastMultipleParmFilled() const {
		/* true if the last parameter
		 * with multiple args
		 * has been filled
		 */
		// is there a multiple parameter ?
		if (! multiple_parm)
			return true;

		return static_cast<int>(current_multiple_args.size()) == multiple_parm->getMax(lang);
	}

	bool ArgCollector::isCurrentParmMultiple() const {
		/* true if the current 
		 * parameter is a multiple one
		 */
		return multiple_parm;
	}

	void ArgCollector::flagNextArgAsNamed(bool b) {
		/* Next arg is supposed to be a named
		 * parameter
		 */
		is_next_arg_named = b;
	}

	void ArgCollector::fillDefaultArgs() {
		/* fill the default args
		 */
		auto unfilled_def { queue.getUnfilledDefault() };
		for (auto& parm_rf : unfilled_def) {
			auto& parm {parm_rf.get()};
			args.at(static_cast<size_t>(parm.getIndex())) = parm.getDefault(lang);
			queue.markFinished(parm);
		}
	}


	void ArgCollector::operator << (const Node& n) {
		collect(n);
	}

	void ArgCollector::operator >> (std::vector<ValVar>& arg_list) const {
		arg_list = getArgs();
	}

	ValVar ArgCollector::format_arg(const Parameter& parm,const Node& n) {
		/* Try to cast the value of the node
		 * into the type expected
		 */
		const ExprType type { parm.getType()};
		try {
			if (type == ExprType::INT) {
				const FString v { remove_trailing_whitespaces(n.getValue()) };
				return text_to<int>(v);
				}
			if (type == ExprType::BOOL) {
				const FString v { remove_trailing_whitespaces(n.getValue()) };
				return text_to<bool>(v);
				}
			if (type != ExprType::TEXT)
				throw BackException(exc::TypeError);

			return n.getValue();

		} catch (const BackException& e) {
			throw iexc(exc::TypeError,"Argument entered does not match the type of the parameter. Argument: ",n.getPosition(),"Parameter: ",parm.getPositions());
		}
	}

	void ArgCollector::finishMultipleArgsAfterLastArg() {
		/* This function sends a signal
		 * to the collector: the multiple parm
		 * must be finished when the next arg
		 * will be received
		 */
		must_finish_mul_parm = true;
	}
}

