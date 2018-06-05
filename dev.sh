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
		inttest.cpp\
		frumul/position.cpp\
		frumul/token.cpp\
		-o inttest\
		$bstrlib/bstrlib.o\
		$bstrlib/bstrwrap.o\
		$bstrlib/buniutil.o\
		$bstrlib/utf8util.o\
}


	

