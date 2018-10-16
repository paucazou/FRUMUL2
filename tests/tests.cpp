#include "tests.h"

namespace ftest {

	void astBrowser(const frumul::Node& parent) {
		/* Takes a node
		 * print it and let the user
		 * choose a child
		 * To select the parent of the parent,
		 * please enter 'parent'
		 */

	back: // there is a goto at the end of the function
		std::cout << parent;
		if (parent.getValue())
			std::cout << "VALUE: " << parent.getValue() << "\n";
		std::cout << "Enter a name of a child, or 'parent' for the parent: ";
		bst::str answer;
		std::cin >> answer;
		if (answer == "parent")
			return;
		
		try {
			if (parent.areChildrenNamed())
				astBrowser(parent.get(answer));
			else 
				astBrowser(parent.get(static_cast<int>(answer)));
		}
		catch (std::out_of_range) {
			std::cerr << "No child of that name\n";
		}
		goto back; // yes, it's bad
	}	

	bool symbolBrowser(const frumul::Symbol& s) {
		/* Takes a symbol and prints
		 * the major informations
		 * about it
		 * if return true, quits
		 */
	back: // yes, another goto, because I'm the worst coder ever !!!
		std::cout << s;
		std::cout << "Enter the name of a child, '--parent','--alias'. 'value' to print value. 'quit' to quit: ";
		bst::str answer;
		std::cin >> answer;
		if (answer == "quit")
			return true;
		else if (answer == "value") {
			std::cout << s.getValue() << std::endl;
		}

		else if (answer == "--parent") {
			if (symbolBrowser(s.getParent()))
				return true;
		}
		else if (answer == "--alias") {
			assert(s.hasAlias()&& "Symbol has no alias");
			assert(s.getAlias()&& "Symbol alias not set");
			symbolBrowser(s.getAlias().getVal());
		}
		else
			symbolBrowser(s.getChildren().getChild(answer));

		goto back;


	}
}
