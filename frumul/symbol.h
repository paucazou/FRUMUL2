#ifndef SYMBOL_H
#define SYMBOL_H
/* This file contains the Symbol
 * definition
 */

#include <cassert>
#include <experimental/any>
#include <functional>
#include <map>
#include <memory>
#include <tuple>
#include "bytecode.h"
#include "exception.h"
#include "macros.h"
#include "mark.h"
#include "name.h"
#include "node.h"
#include "parameters.h"
#include "parser.h"
#include "position.h"
#include "schildren.h"
#include "value.h"
#include "vmtypes.h"

namespace E = std::experimental;

namespace frumul {

	class Parser;
	class Schildren;
	class Symbol;
	class Value;
	struct Arg;

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
			Symbol& getParent();
			Value& getValue();
			Parameters& getParameters();
			std::vector<bst::str> getChildrenNames();
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
			void setParameters(const Parameters&);
			void setReturnType(const Node&);
			// booleans
			bool hasParent() const;
			bool hasAlias () const;
			bool hasMark () const;
			bool hasChildren () const;
			bool hasValue() const;
			// use
			bst::str call(const std::vector<E::any>& args, const bst::str& lang);
			E::any any_call(const std::vector<Arg>& args, const bst::str& lang);
			// display
			bst::str toString() const;
			STDOUT(Symbol)

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

			// functions
			void checkCall(const bst::str& lang); // UNFINISHED TODO arguments must be check


	};

	using RAlias = std::reference_wrapper<Alias>;
	using RSymbol = std::reference_wrapper<Symbol>;
	using CRSymbol = std::reference_wrapper<const Symbol>;
}
#endif
