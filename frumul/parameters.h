#ifndef PARAMATER_H
#define PARAMATER_H
/* This file contains the Parameter
 * class, representing each parameter defined
 * for the values of the symbols
 * Related classes in this file:
 * 	- TextParameter,
 * 	- IntParameter,
 * 	- BoolParameter,
 * 	- SymParameter,
 * 	- Parameters
 */

#include <cassert>
#include <memory>
#include <vector>
#include "bstrlib/bstrwrap.h"
#include "exception.h"
#include "macros.h"
#include "node.h"
#include "position.h"

#define FPARAMATER(F)\
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
			ENUM(Type,FPARAMATER)
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

	class Parameters : public std::vector<Parameter> {
		/* Container of the parameters
		 */
		public:
			Parameters ();
			bool contains(const bst::str& name)const;
			bool operator == (const Parameters& others) const;
		private:
			Parameters& that{*this};
	};

}
#endif
