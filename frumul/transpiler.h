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
			Transpiler(const FString& nsource, const FString& nfilepath,const FString& nlang);

			const FString& getOutput();
			const FString& getLang()const;
			Parser& getParser();
			void append(const FString& part);
		private:
			std::unique_ptr<Parser> parser;
			FString output;
			FString lang;
	};
}
#endif
