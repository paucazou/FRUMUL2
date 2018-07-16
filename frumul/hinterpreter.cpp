#include "hinterpreter.h"

namespace frumul {

	Hinterpreter::Hinterpreter (Node& nheader) :
		header{nheader}
	{
	}

	const Symbol& Hinterpreter::getSymbolTree() {
		/* Interprets the header
		 * and return the symbols
		 * found
		 */
		if (jobDone)
			return main_symbol;
		else {
			for (auto& child : header.getNumberedChildren())
				visit(child);

			jobDone = true;
		}

		return main_symbol;
	}

	void Hinterpreter::visit(Node& node,Symbol& parent) {
		/* This function dispatches
		 * the nodes following their type
		 */
		switch(node.type()) {
			case Node::DECLARATION:
				visit_declaration(node,parent);
				break;

			case Node::OPTIONS:
				visit_options(node,parent);
				break;
			case Node::BASIC_VALUE:
				visit_basic_value(node,parent);
				break;
			case Node::EMPTY:
				// does nothing. What did you expect?
				break;
			default:
				std::cerr << node.toString() << std::endl;
				assert(false&&"No method for this node");
		};
	}

	void Hinterpreter::visit_declaration(Node& node, Symbol& parent) {
		/* Manages the declaration node
		 * TODO only for basic
		 */
		visit(node.get("options"),symbol);
		visit(node.get("value"),symbol);
	}


