#include <cassert>
#include "hinterpreter.h"

namespace frumul {

	Hinterpreter::Hinterpreter (const Node& nheader) :
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
			for (const auto& child : header.getNumberedChildren())
				visit(child,main_symbol);

			jobDone = true;
		}

		return main_symbol;
	}

	void Hinterpreter::visit(const Node& node,Symbol& parent) {
		/* This function dispatches
		 * the nodes following their type
		 */
		switch(node.type()) {
			case Node::DECLARATION:
				visit_declaration(node,parent);
				break;

			case Node::EMPTY:
				// does nothing. What did you expect?
				break;
			default:
				std::cerr << node.toString() << std::endl;
				assert(false&&"No method for this node");
		};
	}

	void Hinterpreter::visit_declaration(const Node& node, Symbol& parent) {
		/* Manages the declaration node
		 * TODO il n'y a que la déclaration basique pour le moment
		 */
		Symbol& symbol { parent.getChildren().getChild(node.get("name")) };
		OneValue& oval{visit_options(node.get("options"),symbol)};
		visit_basic_value(node.get("value"),oval);
		
	}

	OneValue& Hinterpreter::visit_options(const Node& node, Symbol& sym) {
		/* Visit the options node
		 * If mark/lang options are not defined,
		 * set the symbol with parent values
		 */
		std::vector<Lang> langs;
		// visit nodes
		for (auto& elt : node.getNumberedChildren()) {
			if (elt.type() == Node::MARK)
				sym.getMark().set(elt);
			else if (elt.type() == Node::LANG) {
				langs.emplace_back(elt.getValue(),elt.getPosition());
			}
		}


		// check if option lacks
		// TODO

		return sym.getValue().set(langs);
	}	


	void Hinterpreter::visit_basic_value(const Node& node, OneValue& val) {
		/* Fill val
		 * with node
		 * TODO check the node
		 */
		val.setNode(node);
	}
}


