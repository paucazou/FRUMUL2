#include "transpiler.h"

namespace frumul {
	Transpiler::Transpiler(const FString& nsource, const FString& nfilepath, const FString& nlang):
		parser{std::make_unique<Parser>(nsource,nfilepath,*this)}, lang{nlang}
	{
	}

	const FString& Transpiler::getOutput() {
		/* Transpiles if necessary and return the output
		 */
		if (!output)
			// transpile
			parser->parse();
		return output;

	}
	
	const FString& Transpiler::getLang() const {
		return lang;
	}

	Parser& Transpiler::getParser() {
		return *parser;
	}

	void Transpiler::append(const FString& part) {
		/* Append a part to the output
		 */
		output += part;
	}
}
