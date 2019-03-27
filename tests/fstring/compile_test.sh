#!/bin/zsh
frumul_path="../../frumul"
icu_path="$frumul_path/icu"

if [[ $1 ]]; then
	compiler=$1
else
	compiler=g++
fi

$compiler test.cpp \
	-g \
	-std=c++17 \
	-Wno-unused-value \
	$frumul_path/fstring.cpp \
	-I$frumul_path/icu/usr/local/include \
	-I$frumul_path \
	-L$icu_path/usr/local/lib/ \
	-Wl,-R$frumul_path/icu/usr/local/lib/ \
	-licuuc -licudata -licuio -licui18n \
	-DRANDOM_VALUE=${RANDOM} \
	&& valgrind ./a.out
