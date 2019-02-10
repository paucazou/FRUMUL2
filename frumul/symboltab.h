#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H
/* Contains the SymbolTab class,
 * which represents a symbols table
 * and its wrapper
 */

#include <cassert>
#include <list>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include "bytecode.h"
#include "exception.h"
#include "macros.h"
#include "position.h"
#include "vmtypes.h"
namespace frumul {
	class VarSymbol{
		public:
			VarSymbol(const FString& nname,const ExprType& ntype,int nnb,int nscope, const Position& npos);
			// getters
			const FString& getName() const;
			const ExprType& getType() const;
			int getIndex() const;
			const Position& getPosition() const;
			int getScope() const;
			// setters
			FString toString() const;
			STDOUT(VarSymbol)
		private:

			FString name;
			ExprType type;
			const int nb;
			int scope{0};
			Position pos;
	};

	class SymbolTab {
		/* Container of VarSymbol 
		 */
		public:
			SymbolTab();
			// getters
			const VarSymbol& getVarSymbol(const FString& name,bool current_scope_only=false) const;
			VarSymbol& getVarSymbol(const FString& name,bool current_scope_only=false);
			int getIndex(const FString& name) const;
			const ExprType& getType(const FString& name) const;
			bool contains(const FString& name,bool current_scope_only=false) const;
			const Position& getPosition(const FString& name) const;
			int variableNumber() const;
			int getCurrentScope() const;
			// setters
			VarSymbol& append(const VarSymbol& nsymbol);
			VarSymbol& append(const FString& name, const ExprType& type, const Position& pos);
			VarSymbol& append(const FString& name, const ExprType& type, const Position& pos, int scope);
			int& operator++();
			int& operator--();
			static int next();

		private:
			std::list<VarSymbol> content;
			int current_scope{-1};
			int last_scope{-1}; // start at -1 because first one is 0
			std::unordered_map<int,int> scopes; // <child,parent>
	};
}
#endif
