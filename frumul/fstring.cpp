#include <cassert>
#include <codecvt>
#include <locale>
#include "fstring.h"

namespace frumul {
	std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t> utfconverter;

	
	FString::FString():
		_str{icu::UnicodeString()}
	{
		
	}

	FString::FString(const std::string& s) :
		_str{s.data()}
	{
		
	}

	FString::FString(const char * s) :
		_str{s}
	{
		
	}

	FString::FString(const char c) :
		_str{c}
	{
		
	}

	FString::FString(const int i) :
		FString(std::to_string(i).data())
	{
		
	}

	FString::FString(unsigned int i) :
		FString(std::to_string(i).data())
	{
		
	}

	FString::FString(const FString& other) :
		_str{other._str}
	{
		
	}

	FString::FString(const FString&& other) :
		_str{std::move(other._str)}
	{
		
	}

	int FString::length() const {
		return _str.length();
	}

	bool FString::operator == (const FString& other) const {
		return _str == other._str;
	}

	bool FString::operator == (const char other) const {
		return *this == FString(other);
	}

	bool FString::operator != (const FString& other) const {
		return _str != other._str;
	}

	bool FString::operator != (const char other) const {
		return !(*this == other);
	}

	bool FString::operator< (const FString& other) const {
		return _str < other._str;
	}

	char16_t FString::rawAt(int i) const {
		return _str[i];
	}

	FString FString::operator [] (int i) const {
		if (i >= length())
			throw fsexc("FString: index out of range. Index: " + FString(i) + "Length: " + length());
		return utfconverter.to_bytes(rawAt(i));
	}

	FString FString::getLine(int i) const {
		/* Return the line at i
		 * i must be > 0 < linesNumber()
		 */
		assert(i >= 1 &&"Line must be equal or greater than 1");

		FString tmp;
		int linepos{1};
		for (int cpos{0}; linepos <= i && cpos < length(); ++cpos) {
			if (that[cpos] == '\n')
				++linepos;
			else if (linepos == i)
				tmp += that[cpos];
		}
		if (linepos < i || !that)
			throw fsexc("getLine: line requested does not exist");
		return tmp;
	}

	int FString::linesNumber() const {
		/* Return the number of lines in *this
		 */
		int nb{1};
		for (int i{0}; i < length(); ++i) {
			if (that[i] == '\n')
				++nb;
		}
		return nb;
	}

	FString::operator int() const {
		/* Return an int only if every codepoint is a digit
		 */
		std::string str {_check_digits()};

		return std::stoi(str);
	}

	FString::operator unsigned int () const {
		/* Return an unsigned int if every codepoint is
		 * a digit
		 */
		std::string str { _check_digits(false)};
		return std::stoul(str);
	}

	FString::operator bool () const {
		/* true if FString is not empty
		 */
		return !_str.isEmpty();
	}

	FString& FString::operator += (const FString& other) {
		_str += other._str;
		
		return *this;
	}

	FString& FString::operator = (const FString& other) {
		_str = other._str;
		
		return *this;
	}

	FString& FString::toLower() {
		/* To lower case following current locale
		 */
		_str.toLower();
		
		return *this;
	}

	void FString::insert(int pos, const FString& other) {
		/* insert other at pos position
		 */
		assert(pos > -1 && "Insert: pos is under zero");
		assert(pos <= length() && "Insert: pos it too large");
		_str.insert(pos,other._str);
		
	}

	void FString::replace(int pos, const FString& other) {
		/* Replace the char at pos by other
		 */
		assert(pos > -1 && "Replace: pos under zero");
		assert(pos <= length() && "Replace: pos too large");
		constexpr int len {1}; // we don't need more
		_str.replace(pos,len,other._str);
		
	}

	FString FString::extract(int start, int end) const {
		/* Extract a string between start and end (included)
		 * Example :
		 * 	String : "Home, sweet home"
		 * 	extract(0,3) == "Home"
		 * 	extract(4,10) == ", sweet"
		 */
		assert(start > -1 && "Extract: start is under zero");
		assert(end < length() && "Extract: end is too large");
		assert(start <= end && "Extract: start is greater than end");

		FString s;
		int length { end + 1 - start };
		_str.extract(start,length,s._str);
		return s;
	}

	FString::iterator FString::begin() const {
		/* Return an iterator to the beginning of the
		 * string
		 */
		return iterator(*this);
	}

	FString::iterator FString::end() const {
		/* Return an invalid iterator which points just
		 * after the end of the string
		 */
		return iterator(*this,length());
	}

	// iterator
	FString::iterator::iterator(const FString& str, int pos) :
		_pos{pos}, _str{str}
	{
	}

	bool FString::iterator::operator != (const iterator& it) const {
		/* true if it does not point to the same location 
		 * as *this
		 */
		return _pos != it._pos;
	}

	FString FString::iterator::operator * () {
		/* Return a copy of the character pointed
		 */
		assert(*this && "FString::iterator::operator *: invalid iterator");
		return _str[_pos];
	}

	FString::iterator& FString::iterator::operator ++ () {
		++_pos;
		return *this;
	}

	FString::iterator::operator bool () const {
		/* true if iterator is valid
		 */
		return _pos >= 0 && _pos < _str.length();
	}

	int32_t FString::hash() const {
		return _str.hashCode();
	}

	std::ostream& operator << (std::ostream& out, const FString& fs) {
		out << fs._str;
		return out;
	}

	std::istream& operator >> (std::istream& in, FString& fs) {
		in >> fs._str;
		return in;
	}

	std::string FString::_check_digits(bool signed_t) const {
		/* Check that every code point
		 * is a digit and return a string containing
		 * the codepoints
		 */

		constexpr int first_char {0};
		// check that every codepoint is a digit
		for (int i{0}; i < length(); ++i) {
			if (signed_t && i == first_char && (_str[i] == '+' || _str[i] == '-'))
				continue;
			if (!iswdigit(_str[i]))
				throw fsexc("Impossible to cast string to a number. String is: " + that);
		}

		// convert to std::string
		std::string str;
		_str.toUTF8String(str);
		return str;
	}

	// FStringException
	FStringException::FStringException(const FString& s) :
		_msg{s}
	{
	}

	const FString& FStringException::what() const {
		return _msg;
	}

	// other functions
	FString operator + (const FString& s1, const FString& s2) {
		FString returned{s1};
		returned += s2;
		return returned;
	}

	FString operator + (const FString& s, const char * cstring) {
		return s + FString(cstring);
	}

	FString operator + (const char * cstring, const FString& s) {
		return FString(cstring) + s;
	}

	FString operator + (const FString& s, int i) {
		return s + FString (i);
	}

	FString operator + (const FString& s, unsigned int i) {
		return s + FString (i);
	}

	FString operator + (const char c, const FString& s) {
		return FString(c) + s;
	}

	FString operator + (const FString& s, const char c) {
		return s + FString(c);
	}

	FString operator * (const FString& s, int nb) {
		FString ns;
		for (int i{0}; i < nb; ++i)
			ns += s;
		return ns;
	}

	FString operator * (int nb, const FString& s) {
		return s * nb;
	}

	FString operator "" _FS(const char* chars, size_t) {
		return FString(chars);
	}
}

namespace std {
	size_t hash<frumul::FString>::operator () (const frumul::FString& s) const {
		return static_cast<size_t>(s.hash());
	}
}
