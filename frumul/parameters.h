#ifndef FPARAMETER_H
#define FPARAMETER_H
/* This file contains the Parameter
 * class, representing each parameter defined
 * for the values of the symbols
 * Related classes in this file:
 * 	- Parameters
 */


#include <cassert>
#include "valvar.h"
#include <memory>
#include <tuple>
#include <utility>
#include <vector>
#include "fstring.h"
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



namespace frumul {
	class ByteCode;
	class MonoExprCompiler;
	class Symbol;
	class VM;
	struct Arg;

	//using Arg = std::tuple<ExprType,FString,ValVar>; // see vm.h for more

	class Parameter {
		/* One parameter of the values
		 */
		public:
			Parameter(const Node& node,Symbol* np=nullptr);
			Parameter(const Node& node, Symbol& np);
			Parameter(const FString&, const ExprType&,const std::vector<Position>&,Symbol&);
			Parameter(const Parameter&);
			~Parameter();
			// setters
			void appendPos(const Position& npos);
			void setMinMax(const StrNodeMap& fields);
			void setMinMax(int min, int max);
			void setParent(Symbol&);
			void setDefault(const ValVar&);
			void setIndex(int);
			void setChoices(const std::vector<ValVar>&);

			// getters
			bool operator == (const Parameter& other) const;
			bool operator != (const Parameter& other) const;
			const Node& getNodeDefault() const;
			const Node& getChoices() const;
			const ExprType& getType() const;
			const FString& getName() const;
			bool choiceMatch(const ValVar& elt,const FString& lang);
			ValVar getDefault(const FString&);
			int getIndex() const;
			// // min/max
			int getMin(const FString&) ;
			int getMax(const FString&);
			const PosVect& getPositions() const;
			bool operator == (int nb) const;
			bool operator > (int nb) const;
			bool operator < (int nb) const;
			bool between (int nb) const;
			bool between (int nb,const FString&);
			bool hasDefault() const;
			// name
			bool operator == (const FString& n) const;
			// display
			FString toString() const;
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
					int getLimit(const FString& lang,Symbol&) ;
					Comparison getComparison() const;
					bool isConform(int x,const FString&,Symbol&) ;
					const Position& getPosition() const;
					FString toString() const;
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
			FString name;
			std::unique_ptr<Limit> limit1{nullptr};
			std::unique_ptr<Limit> limit2{nullptr};
			int min{-1};
			int max{-1};
			uNode def{nullptr};
			uNode choices{nullptr};
			std::unique_ptr<std::vector<ValVar>> _choices{nullptr};
			std::unique_ptr<ValVar> _def{nullptr};
			std::vector<Position> pos;
			Symbol* parent;
			int index{-1};
			// private functions
			Limit::Comparison comparisonValue(const FString&)const;
			void calculateMinMax(const FString&);
			void calculateMinMaxWithOneLimit(int,Limit::Comparison) ;
			bool _list_match(const ValVar&,const ValVar&, const ExprType&);


	};

	class Parameters {
		/* Container of the parameters
		 */
		public:
			Parameters (Symbol&);
			Parameters();
			Parameters& operator=(const Parameters&);
			bool contains(const FString& name)const;
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
			std::vector<ValVar> formatArgs(const std::vector<Arg>&,const FString&);
		private:
			std::vector<Parameter> parms;
			Symbol* parent;
			ValVar get_multiple_args(const std::vector<Arg>&, size_t&, const FString&,Parameter&);
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
