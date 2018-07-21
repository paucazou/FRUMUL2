#include "value.h"

namespace frumul {
	OneValue::OneValue (const Node& node, std::vector<const bst::str>& nlangs) :
		langs{nlangs}, pos{node.getPosition()}, value{node}
	{
	}

	// getters
	bool OneValue::hasLang (const bst::str& lang) const {
		/* true if lang can be found
		 * in languages 
		 */
		for (const auto& l : langs)
			if (lang == l)
				return true;
		return false;
	}

	const std::vector<const bst::str>& OneValue::getLangs() const {
		/* Get the languages
		 */
		return langs;
	}

	const Position& OneValue::getPosition() const {
		/* Position of the value
		 */
		return pos;
	}

	const Node& OneValue::getValue() const {
		/* Get the value itself
		 */
		return value;
	}

	bst::str OneValue::toString() const {
		/* Representation of the instance
		 */
		bst::str s{"<OneValue>\nLanguages: "};
		for (const auto& l : langs)
			s += l + ". ";
		s += pos.toString();
		return s;
	}

	std::ostream& operator<< (std::ostream& out, const OneValue val) {
		/* Print val on stdout
		 */
		out << val.toString();
		return out;
	}

	// Value
	Value::Value ()
	{
	}

	Value::operator bool() const {
		/* true if it has at least one value
		 */
		return !values.empty();
	}

	const std::vector<const bst::str> Value::getLangs() const {
		/* Return a vector of languages
		 * in which the value can be translated
		 */
		std::vector<const bst::str> langs;
		for (const auto& v : values)
			for (const auto& l : v.getLangs())
				langs.push_back(l);
		return langs;
	}

	bool Value::hasLang(const bst::str& lang) const {
		/* true if lang can be found
		 */
		for (const auto& v : values)
			if (v.hasLang(lang))
				return true;
		return false;
	}

	bool Value::hasEvery() const {
		/* true if every can be found
		 */
		return hasLang("every");
	}

	void Value::set(const Node& val, std::vector<const bst::str> nlangs) {
		/* Create a new value
		 */
		values.emplace_back(val,nlangs);
	}

	bst::str Value::toString() const {
		/* representation of Value
		 */
		bst::str s{"<Value>\n"};
		for (const auto& v : values)
			s += v.toString();
		return s;
	}

	std::ostream& operator<<(std::ostream& out, const Value& val) {
		/* Value to stdout
		 */
		out << val.toString();
		return out;
	}


}
