#include "token.h"
namespace frumul {
#ifdef DEBUG
	Token::Token () :
		type{MAX_TYPES_TEXT}, value{"No value"}, position{1,1,1,1,"Nofile","Nocontent"}
	{}
#endif
	Token::Token (Type ntype, const bst::str& nvalue, const Position& pos) :
		type{ntype}, value{nvalue}, position{pos}
	{}

	Token::Token (Type ntype, const bst::str& nvalue,
			int c1, int l1, int c2, int l2,
			const bst::str& filepath, const bst::str& filecontent):
		type{ntype}, value{nvalue}, position{c1,l1,c2,l2,filepath,filecontent}
	{}

	Token::Token (Type ntype, const bst::str& nvalue,
			const Point& start, const Point& end,
			const bst::str& filepath, const bst::str& filecontent):
		type{ntype}, value{nvalue}, position{start,end,filepath,filecontent}
	{}

	const Position& Token::getPosition () const {
		return position;
	}

	Token::Type Token::getType () const {
		return type;
	}

	const bst::str& Token::getValue () const {
		return value;
	}

	bst::str Token::toString () const {
		bst::str returned{"<Token|" + typeToString(type) + ">\n"};
		returned += position.toString();
		return returned;
	}

#if 0
	// DEPRECATED
	bst::str Token::typeToString (Token::Type t) {
		// TODO stays empty until all things are done
		return bst::str{static_cast<int>(t)};
	}
#endif

	std::ostream& operator << (std::ostream& out, const Token& tok) {
			out << tok.toString();
			return out;
	}
				
} // namespace
