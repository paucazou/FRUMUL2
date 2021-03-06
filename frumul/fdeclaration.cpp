#include "fdeclaration.h"

namespace frumul {
	Fdeclaration::Fdeclaration(Symbol& ns) :
		namespac{ns}
	{
	}

	void Fdeclaration::newName(const Node& node) {
		/* add a new name from node
		 * node must be a LINKED_NAMES, SHORT_NAME
		 * or LONG_NAME
		 */
		assert(!isReady()&&"Declaration has been declared ready to use");
		Symbol& s { namespac.getChildren().getChild(node) };
		// check if a name has been declared twice (or more)
		for (const auto& n : names)
			if (n || s.getName()) {
				IW(W::NamesSetTwice,
						"Name set twice: ", node.getPosition(),
						"Already set here: ", n.getBothPositions());
			}
		names.emplace_back(node);
	}

	void Fdeclaration::declareReady() {
		/* Declare an instance ready to be used
		 */
		used.resize(names.size());
		for (size_t i{0}; i<used.size();++i)
			used[i] = false;
	}

	Fdeclaration::operator bool() const {
		/* true if Fdeclaration is ready
		 */
		return isReady();
	}

	bool Fdeclaration::isReady() const {
		/* true if Fdeclaration is ready
		 * to use
		 */
		return !used.empty();
	}

	std::vector<Name> Fdeclaration::notUsed() const {
		/* Return a vector of Name not used
		 */
		std::vector<Name> list;
		for (size_t i{0}; i<used.size();++i)
			if (!used[i])
				list.push_back(names[i]);
		return list;
	}

	bool Fdeclaration::match(const Node& candidate) {
		/* Checks if node (LONG_NAME or SHORT_NAME)
		 * match with one of the names in the declaration
		 */
		
		// check node type
		assert((candidate.type() == Node::LONG_NAME || candidate.type() == Node::SHORT_NAME)&&"Node must be a short or a long name");
		// check if instance is ready to use
		assert(isReady()&&"Fdeclaration has not yet been declared ready to use");
		// iterates on names
		bool hasMatched{false};
		for (size_t i{0}; i < names.size(); ++i) {
			if (names[i] == candidate.getValue()) {
				used[i] = true;
				hasMatched = true;
			}
		}
		if (!hasMatched)
			throw exc(exc::NoForwardDeclaration,"This name has not been declared before.",candidate.getPosition());
		return hasMatched;

	}

}
