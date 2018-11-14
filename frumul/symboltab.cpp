#include "symboltab.h"

#define ASSERT_NO_NAME \
		assert(false&&"No symbol matching with name"); \
		/* this line will never be used, but... -Wreturn-type... */\
		return content[0]


namespace frumul {
	// VarSymbol
	VarSymbol::VarSymbol(const bst::str& nname, const ExprType& ntype, int nnb, int nscope, const Position& npos) :
		name{nname}, type{ntype}, nb{nnb}, scope{nscope}, pos{npos}
	{
	}

	const bst::str& VarSymbol::getName() const {
		return name;
	}

	const ExprType& VarSymbol::getType() const {
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
		s += type.toString();
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
		if (current_scope < 0)
			throw BackException(exc::VarSymbolUnknown);

		int floor_scope {(current_scope_only ? current_scope : 0)};

		for (int scope{current_scope}; scope >= floor_scope; scope = scopes.at(scope))
			for (auto& s : content)
				if (s.getName() == name && s.getScope() == scope)
					return s;

		throw BackException(exc::VarSymbolUnknown);
	}

	const VarSymbol& SymbolTab::getVarSymbol(const bst::str& name,bool current_scope_only) const{
		if (current_scope < 0)
			throw BackException(exc::VarSymbolUnknown);

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

	const ExprType& SymbolTab::getType(const bst::str& name) const {
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

	VarSymbol& SymbolTab::append(const bst::str& name, const ExprType& type, const Position& pos) {
		/* Create a new symbol in current scope
		 */
		return append(name,type,pos,current_scope);
	}

	VarSymbol& SymbolTab::append(const bst::str& name, const ExprType& type, const Position& pos, int scope) {
		/* Create a new symbol and install it
		 * Please call contains before
		 * Note that this should not be done, except for a special reason.
		 * You should instead call the overloaded function
		 * with automatic management of the scope.
		 */
		/* Why check the content size? Because when adding
		 * the return symbol, the scope is equal to -1, which causes a problem
		 */
		assert((content.size() == 0 || !contains(name,true)) && "Name seems to be already used");
		int nb {static_cast<int>(content.size())}; // new element has the same index as the size of the vector before inserting it
		content.emplace_back(name,type,nb,scope,pos);
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
