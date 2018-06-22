#!/bin/zsh
bstrlib=frumul/bstrlib
compile () {
	if [[ $1 == 'clang' ]]; then
		compiler=$1
		shift
	else
		compiler='g++'
	fi
	$compiler -std=c++11\
		-Wextra -Wall -pedantic\
		-D DEBUG\
		tests/astbrowser.cpp\
		frumul/position.cpp\
		frumul/exception.cpp\
		frumul/token.cpp\
		frumul/lexer.cpp\
		frumul/node.cpp\
		frumul/parser.cpp\
		-o out\
		$bstrlib/bstrlib.o\
		$bstrlib/bstrwrap.o\
		$bstrlib/buniutil.o\
		$bstrlib/utf8util.o\
}


	

