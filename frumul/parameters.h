#ifndef FPARAMETER_H
#define FPARAMETER_H
/* This file contains the Parameter
 * class, representing each parameter defined
 * for the values of the symbols
 * Related classes in this file:
 * 	- Parameters
 */

#include <cassert>
#include <memory>
#include <vector>
#include "bstrlib/bstrwrap.h"
#include "exception.h"
#include "functions.inl"
#include "macros.h"
#include "node.h"
#include "position.h"

#define FPARAMETER(F)\
	F(Text)\
	F(Int)\
	F(Bool)\
	F(Symbol)\
	F(MAX_TYPES)\

namespace frumul {
	class Parameter {
		/* One parameter of the values
		 */
		public:
			ENUM(Type,FPARAMETER)
			Parameter(const Node& node);
			Parameter(const Parameter&);
			~Parameter();
			// setters
			void appendPos(const Position& npos);
			void setMinMax(const StrNodeMap& fields);
			void evaluate();
			void reset();

			// getters
			bool operator == (const Parameter& other) const;
			bool operator != (const Parameter& other) const;
			const Node& getDefault() const;
			const Node& getChoices() const;
			Type getType() const;
			const bst::str& getName() const;
			// // min/max
			int getMin() const;
			int getMax() const;
			const PosVect& getPositions() const;
			bool operator == (int nb) const;
			bool operator > (int nb) const;
			bool operator < (int nb) const;
			bool between (int nb) const;
			// name
			bool operator == (const bst::str& n) const;
			// display
			bst::str toString() const;
			STDOUT(Parameter)
		protected:
			enum Comparison {
				EQUAL,
				SUPERIOR,
				INFERIOR,
				SEQUAL,
				IEQUAL,
			};
			class Limit {
				private:
					Comparison comparison{EQUAL};
					bool isNode{true};
					union {
						Node* node;
						int i;
					};
				public:
					Limit (const Node& n, Comparison c);
					Limit (int ni, Comparison c);
					~Limit();
					int getLimit() const;
					bool isConform(int x) const;
			};
			struct Temp {
				int min;
				int max;
				// TODO
				// default ?????
				// choices ????
#pragma message("Default and choices are not yet set in Temp struct")
			};

			Comparison comparisonValue(const bst::str&)const;
			Type type;
			bst::str name;
			Limit* limit1{nullptr};
			Limit* limit2{nullptr};
			uNode def;
			uNode choices;
			Temp* temporary{nullptr};
			std::vector<Position> pos;

	};

	class Parameters {
		/* Container of the parameters
		 */
		public:
			Parameters ();
			Parameters& operator=(const Parameters&);
			bool contains(const bst::str& name)const;
			bool operator == (const Parameters& others) const;
			PosVect getPositions() const;
			void push_back(const Parameter& np);
			bool empty()const;
			const std::vector<Parameter>& getList()const;
			std::vector<Parameter>& getList();
		private:
			std::vector<Parameter> parms;
	};

}
#endif
