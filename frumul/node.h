#ifndef NODE_H
#define NODE_H

#include "position.h"

namespace frumul {
	class AbstractNode {
		/* Base class
		 * of every node
		 */
		private:
			const Position pos;
		virtual void f() = 0; // TODO delete it when possible
	};

	class DocumentNode : public AbstractNode {
	};

	class HeaderNode : public AbstractNode {
	};

	class StatementListNode : public AbstractNode {
	};

	class TextNode : public AbstractNode {
	};

}// namespace
#endif
