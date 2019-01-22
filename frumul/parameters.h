#ifndef FPARAMETER_H
#define FPARAMETER_H
/* This file contains the Parameter
 * class, representing each parameter defined
 * for the values of the symbols
 * Related classes in this file:
 * 	- Parameters
 */


#include <cassert>
#include <experimental/any>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>
#include "bstrlib/bstrwrap.h"
#include "bytecode.h"
//#include "compiler.h"
#include "exception.h"
#include "functions.inl"
#include "macros.h"
#include "node.h"
#include "position.h"
//#include "symbol.h"
//#include "vm.h"
#include "vmtypes.h"

namespace E = std::experimental;

namespace frumul {
	class ByteCode;
	class MonoExprCompiler;
	class Symbol;
	class VM;
	struct Arg;

	//using Arg = std::tuple<ExprType,bst::str,E::any>; // see vm.h for more

	class Parameter {
		/* One parameter of the values
		 */
		public:
			Parameter(const Node& node,Symbol* np=nullptr);
			Parameter(const Node& node, Symbol& np);
			Parameter(const bst::str&, const ExprType&,const std::vector<Position>&,Symbol&);
			Parameter(const Parameter&);
			~Parameter();
			// setters
			void appendPos(const Position& npos);
			void setMinMax(const StrNodeMap& fields);
			void setParent(Symbol&);
			void setIndex(int);

			// getters
			bool operator == (const Parameter& other) const;
			bool operator != (const Parameter& other) const;
			const Node& getNodeDefault() const;
			const Node& getChoices() const;
			const ExprType& getType() const;
			const bst::str& getName() const;
			bool choiceMatch(const E::any& elt,const bst::str& lang);
			E::any getDefault(const bst::str&);
			int getIndex() const;
			// // min/max
			int getMin(const bst::str&) ;
			int getMax(const bst::str&);
			const PosVect& getPositions() const;
			bool operator == (int nb) const;
			bool operator > (int nb) const;
			bool operator < (int nb) const;
			bool between (int nb) const;
			bool between (int nb,const bst::str&);
			bool hasDefault() const;
			// name
			bool operator == (const bst::str& n) const;
			// display
			bst::str toString() const;
			STDOUT(Parameter)
		protected:
			class Limit {
				public:
					enum Comparison {
						EQUAL,
						SUPERIOR,
						INFERIOR,
						SEQUAL,
						IEQUAL,
					};
					Limit (const Node& n, Comparison c);
					Limit (int ni, Comparison c);
					Limit (const Limit& other);
					~Limit();
					int getLimit(const bst::str& lang,Symbol&) ;
					Comparison getComparison() const;
					bool isConform(int x,const bst::str&,Symbol&) ;
					const Position& getPosition() const;
					bst::str toString() const;
					STDOUT(Limit)
				private:
					Comparison comparison{EQUAL};
					bool isNode{true};
					union {
						Node* node;
						int i;
					};
					Position pos;
			};

			ExprType type;
			bst::str name;
			std::unique_ptr<Limit> limit1{nullptr};
			std::unique_ptr<Limit> limit2{nullptr};
			int min{-1};
			int max{-1};
			uNode def;
			uNode choices;
			std::unique_ptr<std::vector<E::any>> _choices{nullptr};
			std::unique_ptr<E::any> _def{nullptr};
			std::vector<Position> pos;
			Symbol* parent;
			int index{-1};
			// private functions
			Limit::Comparison comparisonValue(const bst::str&)const;
			void calculateMinMax(const bst::str&);
			void calculateMinMaxWithOneLimit(int,Limit::Comparison) ;
			bool _list_match(const E::any&,const E::any&, const ExprType&);


	};

	class Parameters {
		/* Container of the parameters
		 */
		public:
			Parameters (Symbol&);
			Parameters();
			Parameters& operator=(const Parameters&);
			bool contains(const bst::str& name)const;
			bool operator == (const Parameters& others) const;
			PosVect getPositions() const;
			void push_back(const Parameter& np);
			bool empty()const;
			size_t size()const;
			const std::vector<Parameter>& getList()const;
			void setParent(Symbol&);
			std::vector<Parameter>& getList();
			std::vector<Parameter>::iterator begin();
			std::vector<Parameter>::iterator end();
			// use
			std::vector<E::any> formatArgs(const std::vector<Arg>&,const bst::str&);
		private:
			std::vector<Parameter> parms;
			Symbol* parent;
			E::any get_multiple_args(const std::vector<Arg>&, size_t&, const bst::str&,Parameter&);
	};

	using CRParameter = std::reference_wrapper<const Parameter>;
	bool operator == (CRParameter&,CRParameter&);

}
namespace std {
	// hash specialization for CRParameter
	template <>
		class hash<frumul::CRParameter> {
			public:
				size_t operator() (const frumul::CRParameter& p) const {
					// we simply return the adress, as
					// we are sure each address refers
					// to only one object
					return reinterpret_cast<size_t>(&p.get());
				}
		};
}
#endif
