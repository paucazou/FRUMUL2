#include "symboltab.h"

#define ASSERT_NO_NAME \
		assert(false&&"No symbol matching with name"); \
		// this line will never be used, but... -Wreturn-type... \
		return content[0]


namespace frumul {
	// VarSymbol
	VarSymbol::VarSymbol(const bst::str& nname, BT::ExprType ntype, int nnb) :
		name{nname}, type{ntype}, nb{nnb}
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

	//SymbolTab
	
	SymbolTab::SymbolTab() {
	}

	const VarSymbol& SymbolTab::getVarSymbol(const bst::str& name) const {
		/* return symbol requested
		 */
		for (const auto& s : content)
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

	void SymbolTab::append(const VarSymbol& nsymbol) {
		/* Append a new symbol
		 */
		content.push_back(nsymbol);
	}

	void SymbolTab::append(const bst::str& name, BT::ExprType type, int nb) {
		/* Create a new symbol and install it
		 */
		content.emplace_back(name,type,nb);
	}


}
