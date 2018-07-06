#include "position.h"
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
namespace frumul {
	
	// constructors

	Position::Position (int c1, int l1, int c2, int l2, const bst::str& fp, const bst::str& fc) :
		start{getPosFromPoint(c1,l1,fc)}, end{getPosFromPoint(c2,l2,fc)}, filepath{fp}, filecontent{fc}
	{
	}


	Position::Position (const Point& p1, const Point& p2, const bst::str& fp, const bst::str& fc) :
		start{getPosFromPoint(p1,fc)}, end{getPosFromPoint(p2,fc)}, filepath{fp}, filecontent{fc}
	{
	}


	Position::Position (int nstart, int nend, const bst::str& fp, const bst::str& fc) : start{nstart}, end{nend}, filepath{fp}, filecontent{fc}
	{
	}

	// getters
	
	Point Position::getStartPoint() const {
		return getLineColumn(start,filecontent);
	}

	Point Position::getEndPoint() const {
		return getLineColumn(end,filecontent);
	}

	//other functions
	
	bst::str Position::toString () const {
		/* Return a string representation
		 * of the instance
		 * TODO manage \t; color in red the real portion
		 */
		// get the size of the terminal
#ifdef __linux__
		
		winsize term;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &term);
		// to get line: term.ws_row, column: term.ws_col
		int max_col = term.ws_col;
#endif
		// get the color
#ifdef __linux__
		const bst::str red("\033[0;31m");
		const bst::str reset("\033[0m");
#endif

		Point startp {getLineColumn(start,filecontent)};
		Point endp {getLineColumn(end,filecontent)};

		bst::str returned{"File: "};
		returned += filepath + '\n';
		//returned += bst::str{" "} * (startp.getColumn() + bst::str{startp.getLine()}.uLength() ) + "↓\n";

		
		int startcol {startp.getColumn()-1};
		for (int cline{startp.getLine()}; cline <= endp.getLine();++cline) {
			bst::str curline {filecontent.getLine(cline)};
			if (cline == startp.getLine()) {
				if (startcol >= curline.uLength())
					curline += red;
				else
					curline.uInsert(startcol,red);
				if (cline == endp.getLine())
					curline.uInsert(endp.getColumn() + red.uLength(), reset);
				returned += bst::str(cline) + " " + curline + '\n';
				continue;
			}
			else if (cline == endp.getLine()) {
				if (endp.getColumn() >= curline.uLength())
					curline += reset;
				else
					curline.uInsert(endp.getColumn(),reset);
			}

			returned += reset + bst::str(cline) + red + " " + curline + '\n';
		}

		//returned += bst::str{" "} * (endp.getColumn() + bst::str{endp.getLine()}.uLength() ) + "↑\n";

		bst::str temp_line;
		int max_length{0};
		for (int cline{0}; cline <= returned.lineNumber() ; ++cline) {
			temp_line = returned.getLine(cline);
			if (temp_line.uLength() > max_length)
				max_length = temp_line.uLength();
		}
		if (max_length > max_col)
			max_length = max_col;

		bst::str sep{bst::str("=")*max_length + '\n'};
		returned = sep + returned + sep;
		return returned;
	}

	Point Position::getLineColumn (int pos, const bst::str& string) {
		/* Get the line and the column
		 * of pos inside string
		 * If the character at index pos is a newline,
		 * it is considered the end of the line,
		 * not the start of the following one.
		 */
		int line{1};
		int column{(string.uAt(pos) == "\n")};
		int i,lastline; // lastline is the last position of a newline
		for (i=0, lastline=0; i <= pos; ++i)
			if (string.uAt(i) == "\n" && i != pos) {
				++line;
				lastline = i;
			}
		column += pos - lastline + (line == 1); // (line == 1) -> avoid error on first line
		return Point(column,line);
	}

	int Position::getPosFromPoint (const Point& p, const bst::str& string) {
		/* Get the position inside string
		 * from Point
		 */
		int pos{0};
		int lcount{1}; 
		for (auto it{string.begin()}; it != string.end(); ++it, ++pos) {
			if (*it == '\n')
				++lcount;
			if (lcount == p.getLine())
				break;
		}
		return pos -1 + p.getColumn(); // -1: because column starts at 1, not 0
	}

	int Position::getPosFromPoint (const int c, const int l, const bst::str& string) {
		/* Get the position inside string
		 * from c (column) and l (line)
		 */
		return getPosFromPoint(Point{c,l},string);
	}

	// overload
	
	std::ostream& operator<<(std::ostream& out, const Position& pos) {
		out << pos.toString();
		return out;
	}
}//namespace
