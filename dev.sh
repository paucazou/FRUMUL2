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
	$compiler -std=c++11\
		-Wextra -Wall -pedantic\
		-Wfatal-errors\
		-D DEBUG\
		-g\
		tests/astbrowser.cpp\
		frumul/position.cpp\
		frumul/exception.cpp\
		frumul/lexer.cpp\
		frumul/node.cpp\
		frumul/parser.cpp\
		frumul/util.cpp\
		-o out\
		$odir/token.o\
		$bstrlib/bstrlib.o\
		$bstrlib/bstrwrap.o\
		$bstrlib/buniutil.o\
		$bstrlib/utf8util.o\
		/usr/lib/gcc/x86_64-linux-gnu/6/libstdc++fs.a\
		$@
}

objectify () {
	cd $odir
	g++ -std=c++11\
		-Wextra -Wall -pedantic\
		-D DEBUG\
		-c ../$1
	popd
}

	

