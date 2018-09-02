#ifndef SYMBOL_H
#define SYMBOL_H
/* This file contains the Symbol
 * definition
 */

#include <cassert>
#include <functional>
#include <memory>
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

namespace frumul {

	class Schildren;
	class Symbol;

	class Alias {
		/* Simple class which handles
		 * a pointer to another Symbol
		 * and a Position
		 */
		public:
			Alias();
			Alias(const Alias& other);
			//getters
			operator bool () const; // true if it has a valid alias
			bool hasPath() const; // true if path has been set before
			const Position& getPosition() const;
			const Symbol& getVal() const;
			const bst::str& getPath() const;

			// setters
			void setPath(const Node& node);
			void setVal(const Symbol& nalias);
			void setPosition(const Position& npos);
		protected:
			const Symbol* val{nullptr};
			bst::str path;
			std::unique_ptr<Position> pos;
	};

	class Symbol {
		public:
			Symbol ();
			Symbol (const Symbol&);
			//getters
			Name& getName();
			Mark& getMark();
			Alias& getAlias();
			Schildren& getChildren();
			Value& getValue();
			Parameters& getParameters();
			// const getters
			BT::ExprType getReturnType() const;
			const Name& getName() const;
			const Alias& getAlias() const;
			const Schildren& getChildren() const;
			const Symbol& getParent() const;

			// setters
			void setParent(Symbol& nparent);
			void setParameters(const Parameters&);
			// booleans
			bool hasParent() const;
			bool hasAlias () const;
			bool hasMark () const;
			bool hasChildren () const;
			bool hasValue() const;
			// use
			template <typename T>
				T call(const std::vector<E::any>& args) const {
#pragma message "call not ready"
					return T();
				}

			E::any any_call(const std::vector<E::any>& args) const;
			// display
			bst::str toString() const;
			STDOUT(Symbol)

		protected:
			BT::ExprType return_type{BT::TEXT};
			Name name {};
			Mark mark {};
			Schildren children{*this};
			Symbol* parent {nullptr};
			Alias alias;
			Value value;
			Parameters parameters;

	};

	using RAlias = std::reference_wrapper<Alias>;
	using RSymbol = std::reference_wrapper<Symbol>;
	using CRSymbol = std::reference_wrapper<const Symbol>;
}
#endif
