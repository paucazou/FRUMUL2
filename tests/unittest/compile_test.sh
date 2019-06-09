#!/bin/zsh

compiler=$1
if [[ $compiler == "" ]]; then
	compiler="g++"
fi

cd ../../

$compiler -std=c++17 -Wextra -Wall -pedantic -pedantic-errors -Wsign-conversion \
	-Wfatal-errors \
	-L./ \
	-I ./frumul \
	-Wl,-rpath=./ \
	tests/unittest/test.cpp \
	-lfrumul \
	-ldl \
	-o tests/unittest/test.out \
	-isystem frumul/icu/usr/local/include/ \
	-licuuc -licudata -licuio -licui18n \
	-Lfrumul/icu/usr/local/lib \
	-Wl,-Rfrumul/icu/usr/local/lib

tests/unittest/test.out

