#ifndef HEADER_H
#define HEADER_H
/* This is a try to avoid problems
 * linked to circular dependencies
 * This header should be called by 
 * every file
 */
namespace frumul {
	// forward declaration
	class Alias;
	class ByteCode;
	class Compiler;
	class Exception;
	class Fdeclaration;
	class Hinterpreter;
	class Lang;
	class Lexer;
	class Mark;
	class Name;
	class Node;
	class OneValue;
	class Parameter;
	class Parameters;
	class Parser;
	class Position;
	class Schildren;
	class Symbol;
	class Token;
	class Transpiler;
	class Value;
	class VM;
	class Warning;

	using BT = ByteCode;
}
#include <experimental/any>
namespace E = std::experimental;

	#include "bstrlib/bstrwrap.h"
#include "bytecode.h"
	#include "position.h"
#include "value.h"
	#include "macros.h"
#include "name.h"
#include "mark.h"
#include "schildren.h"
	#include "node.h"
#include "parameters.h"
#include "symbol.h"
#include "compiler.h"
	#include "exception.h"
#include "fdeclaration.h"
#include "functions.inl"
#include "hinterpreter.h"
	#include "token.h"
	#include "lexer.h"
#include "parser.h"
	#include "transpiler.h"
	#include "util.h"
#include "vm.h"
#include "warning.h"
#endif
