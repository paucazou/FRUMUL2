#include "position.h"
#include <string>
namespace frumul {
	bst::str Position::toString () const {
		bst::str returned{"File: "};
		returned += filepath + '\n';
		returned += bst::str{" "} * (start.getColumn() + bst::str{start.getLine()}.uLength() ) + "ˇ\n";
		for (int cline{start.getLine()}; cline <= end.getLine();++cline)
			returned += bst::str(cline) + " " + filecontent.getLine(cline) + '\n';
		returned += bst::str{" "} * (end.getColumn() + bst::str{end.getLine()}.uLength() ) + "^\n";

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
}//namespace
