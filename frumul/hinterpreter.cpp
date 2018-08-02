#include <cassert>
#include "hinterpreter.h"
#if DEBUG
#include "../tests/tests.h"
#endif

namespace frumul {
	//InheritedOptions
	
	InheritedOptions::InheritedOptions()
	{
	}

	void InheritedOptions::setLangs(const std::vector<Lang>& nlangs) {
		/* Set langs
		 */
		assert(langs.empty()&&"Langs is not empty");
		for (const auto& elt : nlangs)
			langs.push_back(elt);
	}

	void InheritedOptions::setMark(const Node& node) {
		/* set node
		 */
		mark.set(node);
	}

	const std::vector<Lang>& InheritedOptions::getLangs() const {
		/* Return langs
		 */
		return langs;
	}

	const Mark& InheritedOptions::getMark() const {
		/* return mark
		 */
		return mark;
	}

	// Hinterpreter

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
		/* Simple wrapper
		 */
		Fdeclaration f{parent}; //useless thing
		visit_declaration(node,parent,f);
	}

	void Hinterpreter::visit_declaration(const Node& node, Symbol& parent,Fdeclaration& forward_declaration) {
		/* Manages the declaration node
		 * TODO not yet alias, no inheritance
		 */
		if (forward_declaration) {// we must check if the name has been declared before
			if (!forward_declaration.match(node))
				throw 1;
		}

		// name
		Symbol& symbol { parent.getChildren().getChild(node.get("name")) };
		// using value
		const Node& value{ node.get("value") };
		switch (value.type()) {
			case Node::BASIC_VALUE:
				{
				// options
				OneValue& oval{visit_options_basic(node.get("options"),symbol)};
				visit_basic_value(value,oval);
				}
				break;
			case Node::NAMESPACE_VALUE:
				{
				// options
				visit_options_namespace(node.get("options"));
				Fdeclaration forward {visit_namespace_value(value,symbol)};
				for (const auto& child : node.get("statements").getNumberedChildren())
					visit_declaration(child,symbol,forward);
				
				// check if every forward declaration is followed by a definition
				std::vector<Name> not_used{forward.notUsed()};
				if (!not_used.empty())
					for( const auto& l: not_used)
						IW(W::NameNotUsed,"Forward declaration not followed by a definition.", l.getBothPositions());
				// remove last elt of the stack
				inherited_stack.pop();
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

	OneValue& Hinterpreter::visit_options_basic(const Node& node, Symbol& sym) {
		/* Visit the options node
		 * for basic values
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

	void Hinterpreter::visit_options_namespace(const Node& node) {
		/* Visit the options node
		 * for a namespace values.
		 * These options could be used later
		 * by children
		 */
		
		InheritedOptions io;
		std::vector<Lang> langs;
		// visit nodes
		for (auto& elt : node.getNumberedChildren()) {
			if (elt.type() == Node::MARK)
				io.setMark(elt);
			else if (elt.type() == Node::LANG) {
				langs.emplace_back(elt.getValue(),elt.getPosition());
			}
		}

		// append a new InheritedOptions to the stack
		inherited_stack.push(io);
	}


	void Hinterpreter::visit_basic_value(const Node& node, OneValue& val) {
		/* Fill val
		 * with node
		 * TODO check the node
		 */
		val.setNode(node);
	}
}


