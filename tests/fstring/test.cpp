#include "fstring.h"
#include <cassert>
#include <codecvt>
#include <functional>
#include <locale>
#include <sstream>
#include <string>

#define printl(elt) std::cout << elt << std::endl
#define printerr(elt) std::cerr << elt << std::endl
#define ARRAY_SIZE(a)                               \
     ((sizeof(a) / sizeof(*(a))) /                     \
     static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

using namespace frumul;

std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t> utfconverter;


constexpr char str[] {"ù@à κόσμε блф"};
constexpr int str_size { ARRAY_SIZE(str) };
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
	FString min_one{"-1"};
	assert(static_cast<int>(min_one) == -1);

	FString one_and_more {"1+"};
	try {
		static_cast<int>(one_and_more);
		assert(false&&"Cast shouldn't have been performed");
	} catch (const fsexc& e) {
		// we should land here
	}

	constexpr char err_msg[] { "Cast should'nt have been performed" };
	FString void_and_one {" 1"};
	test_exc<fsexc>([&]() {assert(static_cast<int>(void_and_one) == 1);},err_msg);

	// unsigned int
	FString thousand { "1000" };
	assert(static_cast<unsigned int>(thousand) == 1000);
	test_exc<fsexc>([&]() { assert(static_cast<unsigned int>(min_one) == -1);},err_msg);


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

bool test_lower_than() {
	FString s;
	FString ss;
	assert(s < ss == false);

	s = "0";
	ss = "1";
	assert( s < ss == true);
	assert( ss < s == false);

	s = "01";
	assert( s < ss == true);

	s = "10";
	assert( s < ss == false);
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

	// test const-correctness
	const FString two_lines {"One\nTwo"};
	const FString&& line_got { std::move(two_lines.getLine(2)) };
	assert(&line_got != &two_lines);


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

bool test_lower() {
	// french
	std::locale fr { "fr_FR.UTF-8" };
	std::locale::global(fr);

	FString fr_s { "ÀÉÙËtVv"};
	assert(fr_s.toLower() == "àéùëtvv");

	// russian
	std::locale ru { "ru_RU.UTF-8" }; // you must enable this locale to make the test pass
	std::locale::global(ru);

	FString ru_s { "ЙйАБЦДЕЁР" };
	assert(ru_s.toLower() == "ййабцдеёр" );

	return true;
}

bool test_replace() {
	FString base {"aàa"};
	base.replace(1,"è");
	assert(base == "aèa");

	base.replace(0,"ou");
	assert(base == "ouèa");

	base.replace(3,"ët");
	assert(base == "ouèët");

	return true;
}

bool test_assignment() {
	FString s {str};
	assert(s == str);
	constexpr char new_val[] { "New value" };
	s = new_val;
	assert(s == new_val);
	return true;
}

bool test_extract() {
	FString fs { "Home, sweet home" };
	assert(fs.extract(0,3) == "Home");
	assert(fs.extract(4,10) == ", sweet");
	assert(fs.extract(12,15) == "home");
	
	// test const correctness
	const FString& cfs { fs };
	const FString&& extracted { std::move(cfs.extract(0,3)) };
	assert(&extracted != &cfs);
	return true;
}

bool test_toUTF8() {
	const FString fs{str};
	assert(fs.toUTF8String<std::string>() == str);
	return true;
}

bool test_hash() {
	FString rv { RANDOM_VALUE };
	assert(rv.hash() == std::hash<FString>()(rv));
	return true;
}

bool test_operator_plus() {
	// every argument is supposed to be constant
	const FString fs {"FSTRING"};
	const FString fs2 {"2"};
	// FString + FString
	assert(fs + fs == "FSTRINGFSTRING");
	assert(fs + FString() == fs);
	assert(FString() + fs == fs);
	assert(fs + fs2 != fs2 + fs);

	// FString + const char *
	const char cstring[] {"fstring"};
	assert(fs + "fstring" == "FSTRINGfstring");
	assert(fs + "" == fs);
	assert(FString() + "fstring" == cstring);
	assert(fs + cstring == "FSTRINGfstring");

	// const char * + FString
	assert((cstring + fs) == "fstringFSTRING");
	assert("" + fs == fs);
	assert(cstring + FString() == cstring);
	assert("1" + fs2 == "12");

	// FString + int
	const int i {12};
	assert(fs + i == "FSTRING12");
	assert(fs + 0 == "FSTRING0");
	assert(fs + -1 == "FSTRING-1");

	// FString + unsigned int
	const unsigned int j { 55 };
	assert(fs + j == "FSTRING55");

	// char + FString
	const char c{ 'c' };
	assert(c + fs2 == "c2");
	assert('u' + fs2 == "u2");

	// FString + char
	assert(fs2 + c == "2c");
	assert(fs2 + 'u' == "2u");

	// int * FString
	const int x {2};
	const FString y { "ux" };

	assert(x * y == "uxux");
	assert(3 * y == "uxuxux");
	assert(4 * FString(" ") == "    ");

	// FString * int
	assert( y * x == "uxux");
	assert( y * 3 == "uxuxux");
	assert(FString(" ") * 4 == "    ");
	assert(y * x == x * y);
	
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
	test_toUTF8();

	// // equality
	test_equality();
	// // operator <
	test_lower_than();

	// output
	std::stringstream out;
	out << s;
	assert(out.str() == str);

	// input
	{
		std::stringstream in;
		in.write(str,str_size);
		FString s;
		assert(s == "");
		in >> s;

		std::stringstream in_bis;
		in_bis.write(str,str_size);
		std::string std_s;
		in_bis >> std_s;

		assert(s == std_s.data());
	}
	
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
	test_assignment();
	test_lower();
	test_replace();
	
	// iterators
	test_iterator(s);
	// casts
	test_cast();
	test_hash();

	// FStringException
	test_fstring_exception();

	// operator +
	test_operator_plus();
	

	return 0;
}
