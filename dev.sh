#!/bin/zsh
bstrlib=frumul/bstrlib
odir=odir
compile () {
	if [[ $1 == 'clang' ]]; then
		compiler=$1++
		shift
	else
		compiler='g++'
	fi
	$compiler -std=c++17\
		-Wextra -Wall -pedantic\
		-pedantic-errors\
		-Wfatal-errors\
		-Wsign-conversion\
		-D DEBUG\
		-g\
		tests/astbrowser.cpp\
		tests/tests.cpp\
		frumul/argcollector.cpp\
		frumul/bytecode.cpp\
		frumul/compiler.cpp\
		frumul/exception.cpp\
		frumul/fdeclaration.cpp\
		frumul/fstring.cpp\
		frumul/hinterpreter.cpp\
		frumul/lexer.cpp\
		frumul/mark.cpp\
		frumul/name.cpp\
		frumul/node.cpp\
		frumul/parameters.cpp\
		frumul/parmqueuer.cpp\
		frumul/parser.cpp\
		frumul/position.cpp\
		frumul/retchecker.cpp\
		frumul/schildren.cpp\
		frumul/symbol.cpp\
		frumul/symboltab.cpp\
		frumul/tailresult.cpp\
		frumul/textto.cpp\
		frumul/transpiler.cpp\
		frumul/util.cpp\
		frumul/value.cpp\
		frumul/vm.cpp\
		frumul/vmtypes.cpp\
		frumul/warning.cpp\
		-o out\
		$odir/token.o\
		/usr/lib/gcc/x86_64-linux-gnu/8/libstdc++fs.a\
		-isystem frumul/icu/usr/local/include\
		-isystem frumul/include\
		-licuuc -licudata -licuio -licui18n \
		-Lfrumul/icu/usr/local/lib \
		-Wl,-Rfrumul/icu/usr/local/lib\
		$@
}

objectify () {
	cd $odir
	g++ -std=c++17\
		-Wextra -Wall -pedantic\
		-pedantic-errors\
		-Wsign-conversion\
		-isystem ../frumul/icu/usr/local/include\
		-isystem ../frumul/include\
		-D DEBUG\
		-g\
		-c ../$1
	popd
}

alias check_frumul="cppcheck ./frumul/ --enable=all --inconclusive --force --std=c++11 -i $bstrlib"
alias valgrind="valgrind --leak-check=yes --leak-check=full --show-leak-kinds=all"
	

