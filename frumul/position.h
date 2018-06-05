#ifndef POSITION_H
#define POSITION_H
#include "bstrlib/bstrwrap.h"
namespace frumul {
class Point {
	/* Point handles
	 * two int, column
	 * and line
	 */
	private:
		const int column;
		const int line;
	public:
		//constructors
		Point (int c, int l) : column{c}, line{l}
		{}
		Point (const Point& p) : column{p.column}, line{p.line}
		{}
		//getters
		int getColumn() const {
			return column;
		}
		int getLine() const {
			return line;
		}
};

class Position {
	/* Position is a class which handles
	 * the position inside the file
	 * of many elements, including tokens,
	 * nodes, parts of symbols, etc.
	 */
	private:
		const Point start;	// first char
		const Point end; 	// last char
		const bst::str filepath;
		const bst::str& filecontent;
	public:
		//constructors
		Position (int c1, int l1, int c2, int l2,
			      const bst::str& fp, const bst::str& fc) :
			start{c1,l1}, end{c2,l2}, filepath{fp}, filecontent{fc}
		{}
		Position (int c1, int l1, int c2, int l2,
			       const char *fp, const bst::str& fc) :
			start{c1,l1}, end{c2,l2}, filepath{fp}, filecontent{fc}
		{}
		Position (const Point& p1, const Point& p2, const bst::str& fp, const bst::str& fc):
			start{p1}, end{p2}, filepath{fp}, filecontent{fc}
		{}
		Position (const Point& p1, const Point& p2, const char *fp, const bst::str& fc):
			start{p1}, end{p2}, filepath{fp}, filecontent{fc}
		{}
		Position (const Position& pos) :
			start{pos.start}, end{pos.end}, filepath{pos.filepath}, filecontent{pos.filecontent}
		{}

		//getters
		const Point& getStart()const {
			return start;
		}
		const Point& getEnd() const{
			return end;
		}
		const bst::str & getFilePath () const {
			return filepath;
		}
		// other functions
		bst::str toString () const;
		// overload
		friend std::ostream& operator<<(std::ostream& out, const Position& pos);
};
} // frumul namespace

#endif
