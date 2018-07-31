#include <cassert>
#include "value.h"

namespace frumul {

	// Lang

	Lang::Lang(const bst::str& nname, const Position& npos) :
		name{nname}, pos{npos}
	{
	}

	Lang::Lang(const Lang& other) :
		name{other.name}, pos{other.pos}
	{
	}

	const Position& Lang::getPosition() const {
		return pos;
	}

	const bst::str& Lang::getName() const {
		return name;
	}

	bool Lang::operator == (const bst::str& other) const {
		/* true if other
		 * is equal to name
		 */
		return name == other;
	}

	bool Lang::operator == (const Lang& other) const {
		/* true if other is
		 * equal to this
		 */
		return name == other.name;
	}

	bst::str Lang::toString() const {
		/* Return a string
		 * representation of the instance
		 */
		bst::str s{"<LANG|"};
		s += name + ">\n";
		s += pos.toString();
		return s;
	}

	std::ostream& operator<< (std::ostream& out, const Lang& l) {
		out << l.toString();
		return out;
	}

	
	// OneValue

	OneValue::OneValue()
	{
	}

	OneValue::OneValue(const OneValue& other) :
		langs{other.langs}, value{other.value}
	{
		if (other.pos)
			pos = std::make_unique<Position>(*other.pos);
	}

	OneValue::OneValue(std::vector<Lang>& nlangs) :
		langs{nlangs}
	{
	}

	OneValue::OneValue (const Node& node, std::vector<Lang>& nlangs) :
		langs{nlangs}, pos{std::make_unique<Position>(node.getPosition())}, value{&node}
	{
	}

	// getters
	bool OneValue::hasLang (const bst::str& lang) const {
		/* true if lang can be found
		 * in languages 
		 */
		for (const auto& l : langs)
			if (l == lang)
				return true;
		return false;
	}

	const std::vector<Lang>& OneValue::getLangs() const {
		/* Get the languages
		 */
		return langs;
	}

	const Position& OneValue::getPosition() const {
		/* Position of the value
		 */
		return *pos;
	}

	const Node& OneValue::getValue() const {
		/* Get the value itself
		 */
		return *value;
	}

	// setters

	void OneValue::setNode(const Node& node) {
		/* Set node and position
		 */
		value = &node;
		pos = std::make_unique<Position>(node.getPosition());
	}

	void OneValue::setLangs(std::vector<Lang>& nlangs) {
		/* set langs
		 */
		for (const auto& elt : nlangs)
			langs.push_back(elt);
	}

	// display

	bst::str OneValue::toString() const {
		/* Representation of the instance
		 */
		bst::str s{"<OneValue>\nLanguages: "};
		for (const auto& l : langs)
			s += l.getName() + ".\n";
		if (pos)
			s += pos->toString();
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

	Value::Value(const Value& other) :
		values{other.values}
	{
	}

	Value::operator bool() const {
		/* true if it has at least one value
		 */
		return !values.empty();
	}

	const std::vector<Lang> Value::getLangs() const {
		/* Return a vector of languages
		 * in which the value can be translated
		 */
		std::vector<Lang> langs;
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

	OneValue& Value::set(std::vector<Lang>& nlangs) {
		/* Set a new value with langs only
		 */
		if (interLangs(nlangs,getLangs())) {
			assert(false&&"exception not set");
			throw 1;
		}
		values.emplace_back(nlangs);
		return values.back();
	}
		

	OneValue& Value::set(const Node& val, std::vector<Lang>& nlangs) {
		/* Create a new value
		 */
		OneValue& oval{set(nlangs)};
		oval.setNode(val);
		return oval;
	}

	OneValue& Value::set(const OneValue& val) {
		/* Add a new value
		 */
		if (interLangs(val.getLangs(),getLangs())) {
			assert(false&&"exception not set");
			throw 1;
		}
		values.push_back(val);
		return values.back();
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
