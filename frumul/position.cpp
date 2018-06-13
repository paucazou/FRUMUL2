#include "position.h"
#include <string>
namespace frumul {
	Position::Position (int nstart, int nend, const bst::str& fp, const bst::str& fc) : start{getLineColumn(nstart,fc)}, end{getLineColumn(nend,fc)}, filepath{fp}, filecontent{fc}
	{
	}
	bst::str Position::toString () const {
		bst::str returned{"File: "};
		returned += filepath + '\n';
		returned += bst::str{" "} * (start.getColumn() + bst::str{start.getLine()}.uLength() ) + "↓\n";
		for (int cline{start.getLine()}; cline <= end.getLine();++cline)
			returned += bst::str(cline) + " " + filecontent.getLine(cline) + '\n';
		returned += bst::str{" "} * (end.getColumn() + bst::str{end.getLine()}.uLength() ) + "↑\n";

		bst::str temp_line;
		int max_length{0};
		for (int cline{0}; cline <= returned.lineNumber() ; ++cline) {
			temp_line = returned.getLine(cline);
			if (temp_line.uLength() > max_length)
				max_length = temp_line.uLength();
		}
		bst::str sep{bst::str("=")*max_length + '\n'};
		returned = sep + returned + sep;
		return returned;
	}
	std::ostream& operator<<(std::ostream& out, const Position& pos) {
		out << pos.toString();
		return out;
	}

	Point Position::getLineColumn (int pos, const bst::str& string) {
		/* Get the line and the column
		 * of pos inside string
		 */
		int line{1};
		int column{0};
		int i,lastline; // lastline is the last position of a newline
		for (i=0, lastline=0; i <= pos; ++i)
			if (string.uAt(i) == "\n") {
				++line;
				lastline = i;
			}
		column = pos -lastline;
		return Point(column,line);
	}
}//namespace
