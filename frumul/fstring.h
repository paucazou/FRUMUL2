#ifndef FSTRING_H
#define FSTRING_H

#include <iostream>
#include <string>
#include "unicode/unistr.h"
#include "unicode/ustream.h"

namespace frumul {
	class FString {
		/* Frumul String.
		 * Supports Unicode
		 */
		public:
			// constructors
			FString();
			FString(const std::string&);
			FString(const char*);
			FString(const char);
			FString(const int);
			FString(unsigned int);
			FString(const FString&);
			FString(const FString&&);

			// getters
			int length() const; // WARNING maybe not to be trusted (see ICU doc)
			int linesNumber() const;
			bool operator ==(const FString&) const;
			bool operator == (const char) const;
			bool operator !=(const FString&) const;
			bool operator != (const char) const;
			bool operator < (const FString&) const; 
			char16_t rawAt(int) const;
			FString operator [] (int) const;

			// cast
			operator int ()const;
			operator unsigned int () const; 
			operator bool ()const;
			int32_t hash() const;

			// setters 
			FString& operator += (const FString&);
			void insert(int,const FString&);
			void replace(int,const FString&); 
			FString& operator = (const FString&);
			FString& toLower();

			// to new strings
			FString extract(int,int) const;
			FString getLine(int) const;
			template <typename stringT>
				stringT toUTF8String() const {
					stringT string;
					_str.toUTF8String(string);
					return string;
				}

			// output
			friend std::ostream& operator << (std::ostream&, const FString&);
			friend std::istream& operator >> (std::istream&, FString&);

			// subclasses
			class iterator {
				public:
					iterator(const FString&, int pos = 0);
					bool operator != (const iterator&) const;
					FString operator * ();
					iterator& operator++();
					operator bool () const;
				private:
					int _pos{0};
					const FString& _str;

			};
			
			// iterators
			iterator begin() const;
			iterator end() const;


		private:
			icu::UnicodeString _str ;
			FString& that{*this};

			std::string _check_digits(bool signed_t = true) const; 
	};

	class FStringException {
		public:
			FStringException(const FString&);
			const FString& what() const;
		private:
			const FString _msg;

	};
	FString operator + (const FString&, const FString&);
	FString operator + (const FString&, const char*);
	FString operator + (const char*, const FString&);
	FString operator + (const FString&, int);
	FString operator + (const FString&, unsigned int);
	FString operator + (const char,const FString&);
	FString operator + (const FString&, const char);
	FString operator * (const FString&, int);
	FString operator * (int, const FString&);

	// string litteral which is directly translated into an FString
	// example: "A string"_FS == FString("A string")
	FString operator "" _FS(const char*, size_t);

	using fsexc = FStringException;
	using UnicodePoint = char16_t;
}

namespace std {
	template <> struct hash<frumul::FString>
	{
		size_t operator() (const frumul::FString& s) const;
	};
}
#endif
