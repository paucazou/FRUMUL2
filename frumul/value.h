#ifndef VALUE_H
#define VALUE_H
/* Contains the value of a symbol
 */

#include <cassert>
#include "valvar.h"
#include <functional>
#include <memory>
#include <ostream>
#include <vector>
#include "bytecode.h"
//#include "compiler.h"
#include "node.h"
#include "position.h"
#include "vm.h"



namespace frumul {
	class ValueCompiler;

	class Lang {
		/* Represents one language
		 */
		public:
			Lang(const FString& nname, const Position& npos);
			Lang(const Lang& other);
			// getters
			const Position& getPosition() const;
			const FString& getName() const;
			// booleans
			bool operator==(const FString& other) const;
			bool operator==(const Lang& other) const;
			// display
			FString toString() const;
			friend std::ostream& operator<< (std::ostream& out, const Lang& l);
		private:
			const FString name;
			const Position pos;
	};

	class OneValue {
		/* Represents only one value
		 */
		public:
			using executable = ValVar(const FString&,const std::vector<ValVar>&);
			OneValue(Symbol& nparent);
			~OneValue();
			OneValue(const OneValue& other);
			OneValue(std::vector<Lang>& nlangs,Symbol& nparent);
			OneValue(const Node& node, std::vector<Lang>& nlangs,Symbol& nparent);
			//getters
			bool hasLang(const FString& lang) const;
			const std::vector<Lang>& getLangs() const;
			const Position& getPosition() const;
			const Node& getValue() const;
			Symbol& getParent();
			// setters
			void setNode(const Node& node);
			void setLangs(std::vector<Lang>& nlangs);
			// use
			ValVar execute(const FString& lang,const std::vector<ValVar>& args);
			// display
			FString toString() const;
			friend std::ostream& operator<< (std::ostream& out, const OneValue val);
		private:
			enum __type {
				NODE,
				BYTECODE,
				BINARY,
			};

			std::vector<Lang> langs;
			std::unique_ptr<Position> pos;
			__type type { NODE };
			union {
				const Node * value{nullptr};
				ByteCode * bt;
				//std::function<ValVar(const FString&,const std::vector<ValVar>&)>* bin;
				std::function<executable>* bin;
			};
			Symbol& parent;
	};

	class Value {
		/* Represents the values
		 * of a symbol
		 */
		public:
			Value (Symbol& nparent);
			Value (const Value& other);
			//const FString execute(const FString& lang, args?) const;// execute the value with the arguments. How to do that ?
			ValVar execute(const FString& lang,const std::vector<ValVar>& args);
			// getters
			operator bool () const; // true if value is set
			const std::vector<Lang> getLangs() const;
			const OneValue& getValue(const FString& lang) const;
			OneValue& getValue(const FString& lang,bool every=true);
			bool hasLang(const FString& lang) const;
			bool hasEvery() const;
			bool canExecuteWith(const FString& lang) const;
			//setters
			Value& operator=(const Value&);
			OneValue& set (std::vector<Lang>& nlangs);
			OneValue& set (const Node& val, std::vector<Lang>& nlangs);
			OneValue& set (const OneValue& val);
			// display
			FString toString() const;
			friend std::ostream& operator<<(std::ostream& out, const Value& val);
		private:
			std::vector<OneValue> values;
			Symbol& parent;
	};
	
}
#include "functions.inl"
namespace frumul {
	const auto interLangs { intersection<std::vector<Lang>> };
}
#endif
