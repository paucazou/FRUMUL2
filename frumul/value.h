/* Contains the value of a symbol
 */

#include <ostream>
#include <vector>

namespace frumul {
	class OneValue {
		/* Represents only one value
		 */
		public:
			OneValue(const Node& node, std::vector<const bst::str>& nlangs);
			//getters
			bool hasLang(const bst::str& lang) const;
			const std::vector<const bst::str>& getLangs() const;
			const Position& getPosition() const;
			const Node& getValue() const;
			// display
			bst::str toString() const;
			friend std::ostream& operator<< (std::ostream& out, const OneValue val);
		private:
			std::vector<const bst::str> langs;
			const Position pos;
			const Node value;
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
			const std::vector<const bst::str> getLangs() const;
			bool hasLang(const bst::str& lang) const;
			bool hasEvery() const;
			//setters
			void set (const Node& val, std::vector<const bst::str> nlangs);
			// display
			bst::str toString() const;
			friend std::ostream& operator<<(std::ostream& out, const Value& val);
		private:
			std::vector<OneValue> values;
	};
}
