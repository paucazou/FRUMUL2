#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H
/* Contains the SymbolTab class,
 * which represents a symbols table
 * and its wrapper
 */

#include <cassert>
#include <map>
#include <stdexcept>
#include <vector>
#include "bytecode.h"
#include "exception.h"
#include "position.h"
namespace frumul {
	class VarSymbol{
		public:
			VarSymbol(const bst::str& nname,BT::ExprType ntype,int nnb,const Position& npos);
			// getters
			const bst::str& getName() const;
			BT::ExprType getType() const;
			int getIndex() const;
			bool isDefined() const;
			const Position& getPosition() const;
			// setters
			void markDefined();
		private:

			bst::str name;
			BT::ExprType type;
			int nb;
			bool is_defined{false};
			Position pos;
	};

	class SymbolTab {
		/* Container of VarSymbol 
		 */
		public:
			SymbolTab();
			// getters
			const VarSymbol& getVarSymbol(const bst::str& name) const;
			VarSymbol& getVarSymbol(const bst::str& name);
			int getIndex(const bst::str& name) const;
			BT::ExprType getType(const bst::str& name) const;
			bool contains(const bst::str& name) const;
			bool isDefined(const bst::str& name) const;
			const Position& getPosition(const bst::str& name) const;
			int variableNumber() const;
			// setters
			void append(const VarSymbol& nsymbol);
			void append(const bst::str& name, BT::ExprType type, const Position& pos);
			void markDefined(const bst::str& name);

		private:
			std::vector<VarSymbol> content;
	};
}
#endif
