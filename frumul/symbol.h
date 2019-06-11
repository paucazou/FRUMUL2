#ifndef SYMBOL_H
#define SYMBOL_H
/* This file contains the Symbol
 * definition
 */

#include <cassert>
#include "valvar.h"
#include <functional>
#include <map>
#include <memory>
#include <tuple>
#include "alias.h"
#include "bytecode.h"
#include "exception.h"
#include "macros.h"
#include "mark.h"
#include "name.h"
#include "node.h"
#include "parameters.h"
#include "position.h"
#include "schildren.h"
#include "value.h"
#include "vmtypes.h"



namespace frumul {

	class Parser;
	class Schildren;
	class Symbol;
	class Value;
	struct Arg;


	class Symbol {
		public:
			Symbol ();
			Symbol (const Symbol&);
			virtual ~Symbol() {};
			//getters
			Name& getName();
			Mark& getMark();
			Alias& getAlias();
			Schildren& getChildren();
			Symbol& getParent();
			Value& getValue();
			Parameters& getParameters();
			std::vector<FString> getChildrenNames();
			// const getters
			const ExprType& getReturnType() const;
			const Position& getReturnTypePos() const;
			const Name& getName() const;
			const Alias& getAlias() const;
			const Schildren& getChildren() const;
			const Symbol& getParent() const;
			const Value& getValue() const;
			const Parameters& getParameters() const;

			// setters
			void setParent(Symbol& nparent);
			void changeParent(Symbol& nparent);
			void setParameters(const Parameters&);
			void setReturnType(const Node&);
			void setReturnType(const ExprType&);
			void addUnsafeArgsToParms();
			// booleans
			bool hasParent() const;
			bool hasAlias () const;
			bool hasMark () const;
			bool hasChildren () const;
			bool hasValue() const;
			// use
			virtual FString call(const std::vector<ValVar>& args, const FString& lang);
			virtual ValVar any_call(const std::vector<Arg>& args, const FString& lang);
			// display
			FString toString() const;
			STDOUT(Symbol)
#ifdef DEBUG
			virtual void real_type() const;
#endif

		protected:
			struct {
				ExprType type{ExprType::TEXT};
				std::unique_ptr<Position> pos;
			} return_type;
			Name name {};
			Mark mark {};
			std::unique_ptr<Schildren> children = std::make_unique<Schildren>(*this);
			Symbol* parent {nullptr};
			Alias alias;
			std::unique_ptr<Value> value = std::make_unique<Value>(*this);
			Parameters parameters{*this};
			// state variables
			bool mark_added_to_parameters{false};

			// functions
			void checkCall(const FString& lang); // UNFINISHED TODO arguments must be check


	};

	using RAlias = std::reference_wrapper<Alias>;
	using RSymbol = std::reference_wrapper<Symbol>;
	using CRSymbol = std::reference_wrapper<const Symbol>;
}
#endif
