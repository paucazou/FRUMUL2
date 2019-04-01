#ifndef ALIAS_H
#define ALIAS_H

#include <memory>

#include "fstring.h"
#include "node.h"
#include "position.h"
namespace frumul {
	class Symbol;

	class Alias {
		/* Simple class which handles
		 * a pointer to another Symbol
		 * and a Position
		 */
		public:
			Alias();
			Alias(const Alias& other);
			//getters
			operator bool () const; // true if it has a valid alias
			bool hasPath() const; // true if path has been set before
			const Position& getPosition() const;
			const Symbol& getVal() const;
			const FString& getPath() const;

			// setters
			void setPath(const Node& node);
			void setVal(const Symbol& nalias);
			void setPosition(const Position& npos);
		protected:
			const Symbol* val{nullptr};
			FString path;
			std::unique_ptr<Position> pos;
	};
}
#endif
