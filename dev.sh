#!/bin/zsh
bstrlib=frumul/bstrlib
compile () {
	if [[ $1 == 'clang' ]]; then
		compiler=$1
		shift
	else
		compiler='g++'
	fi
	$compiler -Wextra -Wall\
		inttest.cpp\
		frumul/position.cpp\
		-o inttest\
		$bstrlib/bstrlib.o\
		$bstrlib/bstrwrap.o\
		$bstrlib/buniutil.o\
		$bstrlib/utf8util.o\
}


	

