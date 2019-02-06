#!/bin/zsh
frumul_path="../../frumul"
icu_path="$frumul_path/icu"

g++ test.cpp \
	$frumul_path/fstring.cpp \
	-I$frumul_path/icu/usr/local/include \
	-I$frumul_path \
	-L$icu_path/usr/local/lib/ \
	-Wl,-R$frumul_path/icu/usr/local/lib/ \
	-licuuc -licudata -licuio -licui18n \
	&& valgrind ./a.out
