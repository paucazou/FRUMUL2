#!/bin/zsh
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
		frumul/bstrlib/bstrlib.c\
		frumul/bstrlib/bstrwrap.cpp\
		-o inttest
}


	

