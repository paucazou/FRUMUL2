#ifndef POSITION_H
#define POSITION_H
#include "bstrlib/bstrwrap.h"
namespace frumul {
class Position {
	/* Position is a class which handles
	 * the position inside the file
	 * of many elements, including tokens,
	 * nodes, parts of symbols, etc.
	 */
	private:
		const int column;
		const int line;
		const Bstrlib::CBString filepath;
		const Bstrlib::CBString& filecontent;
	public:
		//constructors
		Position (int c, int l, const Bstrlib::CBString & fp, const Bstrlib::CBString& fc) :
			column{c}, line{l}, filepath{fp}, filecontent{fc}
		{}
		Position (int c, int l, const char *fp, const Bstrlib::CBString& fc) :
			column{c}, line{l}, filepath{*fp}, filecontent{fc}
		{}
		Position (const Position& pos) :
			column{pos.column}, line{pos.line}, filepath{pos.filepath}, filecontent{pos.filecontent}
		{}

		//getters
		int getColumn () const {
			return column;
		}
		int getLine () const {
			return line;
		}
		const Bstrlib::CBString & getFilePath () const {
			return filepath;
		}
		// other functions
		Bstrlib::CBString toString () const;
		// overload
		friend std::ostream& operator<<(std::ostream& out, const Position& pos);
};
} // frumul namespace

#endif
