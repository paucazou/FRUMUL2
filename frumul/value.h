#ifndef VALUE_H
#define VALUE_H
/* Contains the value of a symbol
 */

#include <memory>
#include <ostream>
#include <vector>
#include "node.h"
#include "position.h"

namespace frumul {

	class Lang {
		/* Represents one language
		 */
		public:
			Lang(const bst::str& nname, const Position& npos);
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
			OneValue();
			OneValue(const OneValue& other);
			OneValue(std::vector<Lang>& nlangs);
			OneValue(const Node& node, std::vector<Lang>& nlangs);
			//getters
			bool hasLang(const bst::str& lang) const;
			const std::vector<Lang>& getLangs() const;
			const Position& getPosition() const;
			const Node& getValue() const;
			// setters
			void setNode(const Node& node);
			void setLangs(std::vector<Lang>& nlangs);
			// display
			bst::str toString() const;
			friend std::ostream& operator<< (std::ostream& out, const OneValue val);
		private:
			std::vector<Lang> langs;
			std::unique_ptr<Position> pos;
			const Node * value{nullptr};
	};

	class Value {
		/* Represents the values
		 * of a symbol
		 */
		public:
			Value ();
			//const bst::str execute(const bst::str& lang, args?) const;// execute the value with the arguments. How to do that ?
			// getters
			operator bool () const; // true if value is set
			const std::vector<Lang> getLangs() const;
			bool hasLang(const bst::str& lang) const;
			bool hasEvery() const;
			//setters
			OneValue& set (std::vector<Lang>& nlangs);
			OneValue& set (const Node& val, std::vector<Lang>& nlangs);
			OneValue& set (const OneValue& val);
			// display
			bst::str toString() const;
			friend std::ostream& operator<<(std::ostream& out, const Value& val);
		private:
			std::vector<OneValue> values;
	};
	
}
#include "functions.inl"
namespace frumul {
	const auto interLangs { intersection<std::vector<Lang>> };
}
#endif
