#include "symboltab.h"

#define ASSERT_NO_NAME \
		assert(false&&"No symbol matching with name"); \
		/* this line will never be used, but... -Wreturn-type... */\
		return content[0]


namespace frumul {
	// VarSymbol
	VarSymbol::VarSymbol(const bst::str& nname, BT::ExprType ntype, int nnb, int nscope, const Position& npos) :
		name{nname}, type{ntype}, nb{nnb}, scope{nscope}, pos{npos}
	{
	}

	const bst::str& VarSymbol::getName() const {
		return name;
	}

	BT::ExprType VarSymbol::getType() const {
		return type;
	}

	int VarSymbol::getIndex() const {
		return nb;
	}

	int VarSymbol::getScope() const {
		return scope;
	}

	const Position& VarSymbol::getPosition() const {
		return pos;
	}

	bst::str VarSymbol::toString() const {
		bst::str s{"<VarSymbol|"};
		s += name + bst::str(" - ");
		s += BT::typeToString(type) + ">\n";
		s += "Index: " + bst::str(nb) + '\n';
		s += pos.toString();
		return s;
	}

	//SymbolTab
	
	SymbolTab::SymbolTab() {
	}

	VarSymbol& SymbolTab::getVarSymbol(const bst::str& name, bool current_scope_only) {
		/* return symbol requested
		 */
		int floor_scope {(current_scope_only ? current_scope : 0)};

		for (int scope{current_scope}; scope >= floor_scope; scope = scopes[scope])
			for (auto& s : content)
				if (s.getName() == name && s.getScope() == scope)
					return s;

		throw BackException(exc::VarSymbolUnknown);
	}

	const VarSymbol& SymbolTab::getVarSymbol(const bst::str& name,bool current_scope_only) const{
		int floor_scope {(current_scope_only ? current_scope : 0)};

		for (int scope{current_scope}; scope >= floor_scope; scope = scopes.at(scope))
			for (const auto& s: content)
				if (s.getName() == name && s.getScope() == scope)
					return s;

		throw BackException(exc::VarSymbolUnknown);
	}

	int SymbolTab::getIndex(const bst::str& name) const {
		/* Return the index matching with name
		 */
		return getVarSymbol(name).getIndex();
	}

	BT::ExprType SymbolTab::getType(const bst::str& name) const {
		/* Return the type matching with name
		 */
		return getVarSymbol(name).getType();
	}

	bool SymbolTab::contains(const bst::str& name, bool current_scope_only) const {
		/* true if *this contains name required
		 * and is available in the current scope
		 */
		try {
			getVarSymbol(name,current_scope_only);
			return true;
		} catch (const BackException&){
			return false;
		}
	}

	const Position& SymbolTab::getPosition(const bst::str& name) const {
		/* return position of required value
		 */
		return getVarSymbol(name).getPosition();
	}

	int SymbolTab::variableNumber() const {
		/* Return the number of variables declared
		 */
		return content.size();
	}

	int SymbolTab::getCurrentScope() const {
		/* Return the current scope
		 */
		return current_scope;
	}

	VarSymbol& SymbolTab::append(const VarSymbol& nsymbol) {
		/* Append a new symbol
		 * Please call contains before
		 */
		content.push_back(nsymbol);
		return content.back();
	}

	VarSymbol& SymbolTab::append(const bst::str& name, BT::ExprType type, const Position& pos) {
		/* Create a new symbol and install it
		 * Please call contains before
		 */
		int nb {static_cast<int>(content.size())}; // new element has the same index as the size of the vector before inserting it
		content.emplace_back(name,type,nb,current_scope,pos);
		return content.back();
	}

	int& SymbolTab::operator++() {
		/* Increment the scope
		 */
		int parent {current_scope};
		current_scope = ++last_scope;
		scopes.emplace(current_scope,parent);
		return current_scope;
	}

	int& SymbolTab::operator--() {
		/* Decrement the scope
		 */
		current_scope = scopes[current_scope]; // get the parent scope
		return current_scope;
	}

	int SymbolTab::next() {
		/* This function return an int
		 * incremented by one
		 * Useful for names set by the compiler
		 * which user can not use
		 */
		static int i{0};
		return ++i;
	}


}
