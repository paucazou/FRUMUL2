#!/bin/zsh
bstrlib=frumul/bstrlib
compile () {
	if [[ $1 == 'clang' ]]; then
		compiler=$1
		shift
	else
		compiler='g++'
	fi
	$compiler -Wextra -Wall -pedantic\
		-D DEBUG\
		inttest.cpp\
		frumul/position.cpp\
		frumul/exception.cpp\
		frumul/token.cpp\
		frumul/lexer.cpp\
		-o inttest\
		$bstrlib/bstrlib.o\
		$bstrlib/bstrwrap.o\
		$bstrlib/buniutil.o\
		$bstrlib/utf8util.o\
}


	

