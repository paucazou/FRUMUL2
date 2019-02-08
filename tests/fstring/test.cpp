#include "fstring.h"
#include <cassert>
#include <codecvt>
#include <functional>
#include <locale>
#include <sstream>
#include <string>

#define printl(elt) std::cout << elt << std::endl
#define printerr(elt) std::cerr << elt << std::endl

using namespace frumul;

std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t> utfconverter;


constexpr char str[] {"ù@à κόσμε блф"};
constexpr int str_codes[] {249, 64, 224, 32, 954, 8057, 963, 956, 949, 32, 1073, 1083, 1092};

template <typename classexc>
	bool test_exc(const std::function<void()> fun, const std::string& error_msg) {
		// test supposed to throw exception classexc
		try {
			fun();
			printerr(error_msg);
			assert(false);
		} catch (const classexc&) {
			return true;
		}
		return false;
	}

bool test_constructors() {
	// empty
	FString empty;
	assert(!empty);

	// std::string
	std::string s {str};
	FString std_string{s};
	assert(std_string == s.data());

	// const char *
	const char * chars { str};
	FString from_chars {chars};
	assert(from_chars == str);

	//const char
	char x { 'o' };
	FString from_char {x};
	assert(from_char.length() == 1);
	assert(from_char == x);

	// unsigned int
	unsigned int ui {10};
	FString from_ui { ui };
	assert(from_ui == "10");

	// int
	int i { -5 };
	FString from_i { i };
	assert(from_i == "-5");

	return true;
}

bool test_length(const FString& fs,int ex_len) {
	assert(fs.length() == ex_len);
	return true;
}

bool test_linesNumber(const FString& fs, int ex_lines) {
	assert(fs.linesNumber() == ex_lines);
	return true;
}

bool test_iterator (const FString& fs) {
	assert(fs);

	using iterator = FString::iterator;
	iterator first {fs};
	iterator second {fs,0};
	assert(!(first != second));

	for (int i{0}; i < fs.length(); ++i, ++first) {
		assert(fs[i] == *first); // should be equal
		assert(first); // first should be valid
	}
	assert(!first&&"Should be invalid");
	assert(first == fs.end());

	return true;
}

bool test_cast() {
	// bools
	FString empty;
	assert(static_cast<bool>(empty) == false);

	FString not_empty{"o"};
	assert(static_cast<bool>(not_empty) == true);

	// int
	FString one{"1"};
	assert(static_cast<int>(one) == 1);

	FString one_and_more {"1+"};
	try {
		static_cast<int>(one_and_more);
		assert(false&&"Cast shouldn't have been performed");
	} catch (const fsexc& e) {
		// we should land here
	}
	FString void_and_one {" 1"};

	return true;
}

bool test_fstring_exception () {
	FString msg {"An exception"};

	FStringException exc(msg);
	assert(exc.what() == msg);
	return true;
}

bool test_equality() {
	FString s{str};
	FString ss{str};
	assert((s == ss) == true);
	assert((s != ss) == false);

	ss += "o";
	assert((s == ss) == false);
	assert((s != ss) == true);

	const char c {'o'};
	FString mono {c};
	assert((mono == c) == true);
	assert((mono != c) == false);
	const char cc {'u'};
	assert((mono == cc) == false);
	assert((mono != cc) == true);

	FString ostart{"ou"};
	assert((ostart == c) == false);
	assert((ostart != c) == true);


	return true;
}

bool test_getLine() {
	// empty
	FString empty;
	test_exc<fsexc>([&]() {empty.getLine(1);},"String supposed to be empty");

	// whith no \n
	FString s{"A line"};
	assert(s.linesNumber() == 1);
	FString s1 {s.getLine(1)};
	assert(s1 == s);
	test_exc<fsexc>([&]() { s.getLine(2);},"No line 2");

	// three lines
	FString three {"One\nTwo\nThree"};
	assert(three.getLine(2) == "Two");

	// starting by \n
	FString start_n{"\nTwo"};
	assert(start_n.getLine(1) == "");
	assert(start_n.getLine(2) == "Two");

	// ending by \n
	FString end_n{"One\n"};
	assert(end_n.getLine(1) == "One");
	assert(end_n.getLine(2) == "");

	return true;
}

bool test_plus_equal() {
	FString empty;
	assert(static_cast<bool>(empty) == false);
	empty += "a";
	assert(empty == "a");
	empty += FString("b");
	assert(empty == "ab");

	(empty += "c")+= "d";
	assert(empty == "abcd");

	empty += 1;
	assert(empty == "abcd1");

	return true;
}

bool test_insert() {
	// start
	FString s{str};
	s.insert(0,"À ");
	assert(s == FString("À ") + str);

	// end
	FString ss {str};
	ss.insert(ss.length()," ç");
	assert(ss == FString(str) + " ç");

	// middle
	FString sss {"àèrt"};
	sss.insert(2,"404");
	assert(sss == "àè404rt");

	return true;
}

bool test_extract() {
	FString fs { "Home, sweet home" };
	printl(fs.extract(0,3));
	assert(fs.extract(0,3) == "Home");
	printl(fs.extract(4,10));
	assert(fs.extract(4,10) == ", sweet");
	return true;
}

int main () {

	test_constructors();
	// getters
	// // length
	FString s { str };
	test_length(s,13);

	// // indices
	for (int i{0}; i < s.length(); ++i)
		assert(s.rawAt(i) == str_codes[i]);

	for (int i{0}; i < s.length(); ++i)
		assert(s[i] == FString(utfconverter.to_bytes(str_codes[i])));

	// // lines number
	test_linesNumber(str,1);

	FString two_lines { "first\nsecond" };
	test_linesNumber(two_lines,2);

	FString two_lines_trailing_n { "first\nsecond\n" };
	test_linesNumber(two_lines_trailing_n,3);

	// // getline
	test_getLine();
	test_extract();

	// // equality
	test_equality();
	// output
	std::stringstream out;
	out << s;
	assert(out.str() == str);
	
	// iterators
	// // begin \ end
	assert(s.begin() == FString::iterator(s,0));
	assert(s.end() == FString::iterator(s,s.length()));
	// // for range loop
	{
		int i{0};
		for (const auto elt : s) {
			assert(elt == FString(utfconverter.to_bytes(str_codes[i])));
			++i;
		}
	}

	// setters
	test_plus_equal();
	test_insert();
	
	// iterators
	test_iterator(s);
	// casts
	test_cast();

	// FStringException
	test_fstring_exception();
	

	return 0;
}
