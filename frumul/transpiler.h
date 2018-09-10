#ifndef TRANSPILER_H
#define TRANSPILER_H

#include <memory>
//#include "lexer.h"
#include "parser.h"
//#include "header.h"

namespace frumul {
	class Parser;
	class Transpiler {
		/* Transpiles a string into another one
		 */
		public:
			Transpiler(const bst::str& nsource, const bst::str& nfilepath,const bst::str& nlang);

			const bst::str& getOutput();
			const bst::str& getLang()const;
			Parser& getParser();
			void append(const bst::str& part);
		private:
			std::unique_ptr<Parser> parser;
			bst::str output;
			bst::str lang;
	};
}
#endif
