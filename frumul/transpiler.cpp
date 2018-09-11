#include "transpiler.h"

namespace frumul {
	Transpiler::Transpiler(const bst::str& nsource, const bst::str& nfilepath, const bst::str& nlang):
		parser{std::make_unique<Parser>(nsource,nfilepath,*this)}, lang{nlang}
	{
	}

	const bst::str& Transpiler::getOutput() {
		/* Transpiles if necessary and return the output
		 */
		if (output)
			// transpile
			parser->parse();
		return output;

	}
	
	const bst::str& Transpiler::getLang() const {
		return lang;
	}

	Parser& Transpiler::getParser() {
		return *parser;
	}

	void Transpiler::append(const bst::str& part) {
		/* Append a part to the output
		 */
		output += part;
	}
}
