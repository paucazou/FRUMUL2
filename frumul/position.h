#ifndef POSITION_H
#define POSITION_H
#include <vector>
#include "fstring.h"
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
		const int start;	// first char
		const int end; 		// last char
		const FString& filepath;
		const FString& filecontent;
	public:
		//constructors
		Position (int c1, int l1, int c2, int l2,
			      const FString& fp, const FString& fc);
		Position (const Point& p1, const Point& p2, const FString& fp, const FString& fc);
		Position (int nstart, int nend, const FString& fp, const FString& fc); // nstart and nend are index in fc
		Position (const Position& pos) :
			start{pos.start}, end{pos.end}, filepath{pos.filepath}, filecontent{pos.filecontent}
		{}

		//getters
		int getStart()const {
			return start;
		}
		int getEnd() const{
			return end;
		}
		Point getStartPoint() const;
		Point getEndPoint() const;

		const FString & getFilePath () const {
			return filepath;
		}
		// other functions
		FString toString () const;
		static Point getLineColumn (int pos, const FString& string);
		static int getPosFromPoint (const Point& p, const FString& string);
		static int getPosFromPoint (const int c, const int l, const FString& string);
		// overload
		friend std::ostream& operator<<(std::ostream& out, const Position& pos);
		Position operator+(const Position& other) const;
};

using PosVect = std::vector<Position>;
} // frumul namespace

#endif
