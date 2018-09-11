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

	OneValue::OneValue(Symbol& nparent) :
		parent{nparent}
	{
	}

	OneValue::~OneValue() {
		if (is_byte_code_compiled)
			delete bt;
	}

	OneValue::OneValue(const OneValue& other) :
		langs{other.langs}, value{other.value}, parent{other.parent}
	{
		if (other.pos)
			pos = std::make_unique<Position>(*other.pos);
	}

	OneValue::OneValue(std::vector<Lang>& nlangs,Symbol& nparent) :
		langs{nlangs}, parent{nparent}
	{
		assert(!nlangs.empty()&&"Langs is empty");
	}

	OneValue::OneValue (const Node& node, std::vector<Lang>& nlangs,Symbol& nparent) :
		langs{nlangs}, pos{std::make_unique<Position>(node.getPosition())}, value{&node}, parent{nparent}
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

	// use

	E::any OneValue::execute(const bst::str& lang) {
		/* Execute value
		 */
		// compile if necessary
		if (!is_byte_code_compiled) {
			ValueCompiler compiler{*this};
			ByteCode* _bt { new ByteCode(compiler.compile()) };
			delete value;
			bt = _bt;
			is_byte_code_compiled = true;
		}
		VM vm{*bt,lang};
		return vm.run();
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
	Value::Value (Symbol& nparent):
		parent{nparent}
	{
	}

	Value::Value(const Value& other) :
		values{other.values}, parent{other.parent}
	{
	}

	E::any Value::execute(const bst::str& lang) {
		/* Execute requested value
		 * and return its return value
		 */
		OneValue& val{getValue(lang)};
		return val.execute(lang);
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

	OneValue& Value::getValue(const bst::str& lang,bool every) {
		/* Return requested value or every
		 */
		assert((hasLang(lang) && !every) && "Lang not found");
		// look for requested lang
		for (auto& val : values)
			if (val.hasLang(lang))
				return val;
		assert(hasEvery()&&"Every not set");
		// look for every
		for (auto& val : values)
			if (val.hasLang("every"))
				return val;
		return values[0];// -Wreturn-type !!!, and dangerous
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

	bool Value::canExecuteWith(const bst::str& lang) const {
		/* true if lang requested can match a lang
		 * or value has every
		 */
		return hasLang(lang) || hasEvery();
	}

	OneValue& Value::set(std::vector<Lang>& nlangs) {
		/* Set a new value with langs only
		 */
		if (interLangs(nlangs,getLangs())) {
			assert(false&&"exception not set");
#pragma message "Exception not set"
			throw 1;
		}
		values.emplace_back(nlangs,parent);
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
