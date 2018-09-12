#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H
/* Contains the SymbolTab class,
 * which represents a symbols table
 * and its wrapper
 */

#include <cassert>
#include <vector>
#include "bytecode.h"
#include "exception.h"
namespace frumul {
	class VarSymbol{
		public:
			VarSymbol(const bst::str& nname,BT::ExprType ntype,int nnb);
			// getters
			const bst::str& getName() const;
			BT::ExprType getType() const;
			int getIndex() const;
		private:
			bst::str name;
			BT::ExprType type;
			int nb;
	};

	class SymbolTab {
		/* Container of VarSymbol 
		 */
		public:
			SymbolTab();
			// getters
			const VarSymbol& getVarSymbol(const bst::str& name) const;
			int getIndex(const bst::str& name) const;
			BT::ExprType getType(const bst::str& name) const;
			bool contains(const bst::str& name) const;
			// setters
			void append(const VarSymbol& nsymbol);
			void append(const bst::str& name, BT::ExprType type, int nb);
		private:
			std::vector<VarSymbol> content;
	};
}
#endif
