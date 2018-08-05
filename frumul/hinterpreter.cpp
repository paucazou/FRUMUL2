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

	bool InheritedOptions::hasMark() const {
		/* true if it contains a
		 * mark
		 */
		return mark.get();
	}

	bool InheritedOptions::hasLangs() const {
		/* true if it contains langs
		 */
		return !langs.empty();
	}

	bst::str InheritedOptions::toString() const {
		/* string representation
		 */
		bst::str s{"<InheritedOptions"};
		if (hasMark())
			s += "|Mark";
		if (hasLangs())
			s+= "|Langs";
		s+= ">\n";

		if (hasMark())
			s += mark.toString();
		if (hasLangs())
			for (const auto& l : langs)
				s += l.toString();
		return s;
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
			// create the Symbol tree
			for (const auto& child : header.getNumberedChildren())
				visit(child,main_symbol);

			// set the aliases
			interpret_aliases();
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
		 * TODO not yet alias, 
		 */
		if (forward_declaration) {// we must check if the name has been declared before
			forward_declaration.match(node.get("name"));
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
			case Node::ALIAS_VALUE:
				{
					// check that no option has been declared
					if (node.get("options").type() != Node::EMPTY)
						throw exc(exc::SyntaxError,"No option allowed before an alias value",node.get("options").getPosition());

					// set positions and path
					Alias& alias {symbol.getAlias()};
					alias.setPath(value);

					// add alias to the stack in order to interpret the path
					aliases.push(std::ref(symbol));

				}
				break;
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


		// check if option lacks and return
		return inherit(node,sym,langs);
	}	

	OneValue& Hinterpreter::inherit(const Node& node,Symbol& sym,std::vector<Lang>& langs) {
		/* Checks if options lacks and inherit
		 * them from parents
		 */
		assert((node.type() == Node::OPTIONS||node.type() == Node::EMPTY)&&"Node is not an option");
		// mark
		if (!sym.getMark()()) {
			try {
				for (int i{0};; ++i){
					InheritedOptions& io {inherited_stack.topMin(i)};
					if (io.hasMark()) {
						sym.getMark().set(io.getMark());
						break;
					}
				}
			} catch (std::out_of_range){
				throw exc(exc::RequiredOptionNotSet,"Mark option not set nor inherited",node.getPosition());
			}
		}

		// langs
		if (langs.empty()) {
			try {
				for (int i{0};; ++i){
					InheritedOptions& io{inherited_stack.topMin(i)};
					if (io.hasLangs()) {
						for(const auto& l : io.getLangs())
							langs.push_back(l);
						break;
					}
				}
			} catch (std::out_of_range) {
				throw exc(exc::RequiredOptionNotSet,"Lang option not set nor inherited",node.getPosition());
			}
		}
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
		io.setLangs(langs);

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

	void Hinterpreter::interpret_aliases() {
		/* This function interprets the aliases
		 * found in the AST and set the pointer in them
		 */

		while (!aliases.empty()) {
			// get the reference
			Symbol& symbol_alias {aliases.top().get()};
			Schildren& children {symbol_alias.getChildren()};
			Alias& alias {symbol_alias.getAlias()};
			// set the alias
			try {
				alias.setVal(children.find(alias.getPath(),Schildren::Relative));
			} catch (bst::str& e) {
				throw exc(exc::SymbolNotFound,bst::str("No symbol found for this path: ") + e,alias.getPosition());
			}
			// remove alias;
			aliases.pop();

		}

	}
}


