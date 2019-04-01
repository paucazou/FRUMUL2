#include "alias.h"
#include <cassert>
#include "exception.h"

namespace frumul {
	// alias
	Alias::Alias()
	{
	}

	Alias::Alias(const Alias& other) :
		val{other.val}, path{other.path}
	{
		assert(!val&&"Doesn't work, because the pointer points to garbage...");
		if (other.pos)
			pos = std::make_unique<Position>(*other.pos);
	}

	Alias::operator bool () const {
		/* true if it is a valid and usable
		 * alias
		 */
		return val;
	}

	bool Alias::hasPath() const {
		/* true if a path has already been set
		 */
		return path;
	}

	const Position& Alias::getPosition() const {
		/* return the position of the alias
		 */
		return *pos;
	}

	const Symbol& Alias::getVal() const {
		/* Unsafe function
		 * that get the symbol to which points
		 * the alias.
		 */
		assert(val&&"Pointer is not set");
		return *val;
	}

	const FString& Alias::getPath() const {
		/* return path
		 * value
		 */
		return path;
	}

	void Alias::setPath(const Node& node) {
		/* save the path which points
		 * to the symbol.
		 * Sets the position with it.
		 */
		assert(node.type() == Node::ALIAS_VALUE&&"node has not required type: ALIAS_VALUE");
		if (path)
			throw iexc(exc::AliasAlreadySet,"You can't set two alias to one symbol.",node.getPosition(),"Alias already set here:",*pos);

		path = node.getValue(); 
		setPosition(node.getPosition());
	}

	void Alias::setVal(const Symbol& nalias) {
		/* set the pointer to symbol
		 * Please check before that the pointer
		 * has been set yet.
		 */
		assert(!val&&"Alias pointer already set");
		val = &nalias;
	}

	void Alias::setPosition(const Position& npos) {
		/*Set the pointer to position
		 */
		assert(!pos&&"Position of the alias has already been set");
		pos = std::make_unique<Position>(npos);
	}

}
