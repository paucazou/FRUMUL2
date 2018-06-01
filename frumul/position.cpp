#include "position.h"
#include <string>
namespace frumul {
	Bstrlib::CBString Position::toString () const {
		Bstrlib::CBString str{"File: "};
		str += filepath + '\n';
		str += "Line: " + Bstrlib::CBString{line};
		str += ", Column: " + Bstrlib::CBString{column};
		str += "\n";
		str += filecontent.getLine(line);
		str += "\n" + Bstrlib::CBString(" ")*(column - 1) + "^\n";
		return str;
	}
	std::ostream& operator<<(std::ostream& out, const Position& pos) {
		out << pos.toString();
		return out;
	}
}//namespace
