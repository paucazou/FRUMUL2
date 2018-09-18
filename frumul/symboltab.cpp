#include "symboltab.h"

#define ASSERT_NO_NAME \
		assert(false&&"No symbol matching with name"); \
		/* this line will never be used, but... -Wreturn-type... */\
		return content[0]


namespace frumul {
	// VarSymbol
	VarSymbol::VarSymbol(const bst::str& nname, BT::ExprType ntype, int nnb, const Position& npos) :
		name{nname}, type{ntype}, nb{nnb}, pos{npos}
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

	bool VarSymbol::isDefined() const {
		/* true if symbol has been defined at least once
		 */
		return is_defined;
	}

	const Position& VarSymbol::getPosition() const {
		return pos;
	}

	void VarSymbol::markDefined() {
		/* mark the symbol as defined at least once
		 */
		is_defined = true;
	}

	//SymbolTab
	
	SymbolTab::SymbolTab() {
	}

	VarSymbol& SymbolTab::getVarSymbol(const bst::str& name) {
		/* return symbol requested
		 */
		for (auto& s : content)
			if (s.getName() == name)
				return s;
		throw BackException(exc::VarSymbolUnknown);
	}

	const VarSymbol& SymbolTab::getVarSymbol(const bst::str& name) const{
		for (const auto& s: content)
			if (s.getName() == name)
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

	bool SymbolTab::contains(const bst::str& name) const {
		/* true if *this contains name required
		 */
		try {
			getVarSymbol(name);
			return true;
		} catch (const BackException&){
			return false;
		}
	}

	bool SymbolTab::isDefined(const bst::str& name) const {
		/* true if symbol required is defined
		 */
		return getVarSymbol(name).isDefined();
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

	VarSymbol& SymbolTab::append(const VarSymbol& nsymbol) {
		/* Append a new symbol
		 */
		content.push_back(nsymbol);
		return content.back();
	}

	VarSymbol& SymbolTab::append(const bst::str& name, BT::ExprType type, const Position& pos) {
		/* Create a new symbol and install it
		 */
		int nb {static_cast<int>(content.size())}; // new element has the same index as the size of the vector before inserting it
		content.emplace_back(name,type,nb,pos);
		return content.back();
	}

	void SymbolTab::markDefined(const bst::str& name) {
		/* Mark defined requested name
		 */
		getVarSymbol(name).markDefined();
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
