#ifndef VALUE_H
#define VALUE_H
/* Contains the value of a symbol
 */

#include <cassert>
#include <experimental/any>
#include <memory>
#include <ostream>
#include <vector>
#include "bytecode.h"
//#include "compiler.h"
#include "node.h"
#include "position.h"
#include "vm.h"

namespace E = std::experimental;

namespace frumul {
	class ValueCompiler;

	class Lang {
		/* Represents one language
		 */
		public:
			Lang(const bst::str& nname, const Position& npos);
			Lang(const Lang& other);
			// getters
			const Position& getPosition() const;
			const bst::str& getName() const;
			// booleans
			bool operator==(const bst::str& other) const;
			bool operator==(const Lang& other) const;
			// display
			bst::str toString() const;
			friend std::ostream& operator<< (std::ostream& out, const Lang& l);
		private:
			const bst::str name;
			const Position pos;
	};

	class OneValue {
		/* Represents only one value
		 */
		public:
			OneValue(Symbol& nparent);
			~OneValue();
			OneValue(const OneValue& other);
			OneValue(std::vector<Lang>& nlangs,Symbol& nparent);
			OneValue(const Node& node, std::vector<Lang>& nlangs,Symbol& nparent);
			//getters
			bool hasLang(const bst::str& lang) const;
			const std::vector<Lang>& getLangs() const;
			const Position& getPosition() const;
			const Node& getValue() const;
			Symbol& getParent();
			// setters
			void setNode(const Node& node);
			void setLangs(std::vector<Lang>& nlangs);
			// use
			E::any execute(const bst::str& lang,const std::vector<E::any>& args);
			// display
			bst::str toString() const;
			friend std::ostream& operator<< (std::ostream& out, const OneValue val);
		private:
			std::vector<Lang> langs;
			std::unique_ptr<Position> pos;
			bool is_byte_code_compiled {false};
			union {
				const Node * value{nullptr};
				ByteCode * bt;
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
			//const bst::str execute(const bst::str& lang, args?) const;// execute the value with the arguments. How to do that ?
			E::any execute(const bst::str& lang,const std::vector<E::any>& args);
			// getters
			operator bool () const; // true if value is set
			const std::vector<Lang> getLangs() const;
			const OneValue& getValue(const bst::str& lang) const;
			OneValue& getValue(const bst::str& lang,bool every=true);
			bool hasLang(const bst::str& lang) const;
			bool hasEvery() const;
			bool canExecuteWith(const bst::str& lang) const;
			//setters
			OneValue& set (std::vector<Lang>& nlangs);
			OneValue& set (const Node& val, std::vector<Lang>& nlangs);
			OneValue& set (const OneValue& val);
			// display
			bst::str toString() const;
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
