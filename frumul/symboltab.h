#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H
/* Contains the SymbolTab class,
 * which represents a symbols table
 * and its wrapper
 */

#include <cassert>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include "bytecode.h"
#include "exception.h"
#include "macros.h"
#include "position.h"
namespace frumul {
	class VarSymbol{
		public:
			VarSymbol(const bst::str& nname,BT::ExprType ntype,int nnb,int nscope, const Position& npos);
			// getters
			const bst::str& getName() const;
			BT::ExprType getType() const;
			int getIndex() const;
			const Position& getPosition() const;
			int getScope() const;
			// setters
			bst::str toString() const;
			STDOUT(VarSymbol)
		private:

			bst::str name;
			BT::ExprType type;
			int nb;
			int scope{0};
			Position pos;
	};

	class SymbolTab {
		/* Container of VarSymbol 
		 */
		public:
			SymbolTab();
			// getters
			const VarSymbol& getVarSymbol(const bst::str& name,bool current_scope_only=false) const;
			VarSymbol& getVarSymbol(const bst::str& name,bool current_scope_only=false);
			int getIndex(const bst::str& name) const;
			BT::ExprType getType(const bst::str& name) const;
			bool contains(const bst::str& name,bool current_scope_only=false) const;
			const Position& getPosition(const bst::str& name) const;
			int variableNumber() const;
			int getCurrentScope() const;
			// setters
			VarSymbol& append(const VarSymbol& nsymbol);
			VarSymbol& append(const bst::str& name, BT::ExprType type, const Position& pos);
			int& operator++();
			int& operator--();
			static int next();

		private:
			std::vector<VarSymbol> content;
			int current_scope{-1};
			int last_scope{-1}; // start at -1 because first one is 0
			std::unordered_map<int,int> scopes; // <child,parent>
	};
}
#endif
