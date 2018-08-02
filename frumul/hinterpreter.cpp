#include <cassert>
#include "hinterpreter.h"
#if DEBUG
#include "../tests/tests.h"
#endif

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
		if (!jobDone) {
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
		// using value
		const Node& value{ node.get("value") };
		switch (value.type()) {
			case Node::BASIC_VALUE:
				visit_basic_value(value,oval);
				break;
			case Node::NAMESPACE_VALUE:
				{
				Fdeclaration forward {visit_namespace_value(value,symbol)};
				}
				break;
			//case Node::ALIAS_VALUE:
			//	break;
			default:
				std::cerr<<value.toString() << std::endl;
				assert(false&&"The node is not a value");
		};

		// set parent if necessary
		if (!symbol.hasParent())
			symbol.setParent(parent);
		
	}

	Fdeclaration Hinterpreter::visit_namespace_value(const Node& val, Symbol& parent) {
		/* Interprets the namespace value
		 * and creates a forword declaration object
		 */
		Fdeclaration forward{parent};
		for (const auto& node_name : val.getNumberedChildren())
			forward.newName(node_name);
		forward.declareReady();
		return forward;
	}

	OneValue& Hinterpreter::visit_options(const Node& node, Symbol& sym) {
		// TODO pour le moment, ça ne fonctionne pas pour les espaces de noms
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


