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
	-D DEBUG\
	-std=c++17 \
	-Wno-unused-value \
	-Wall -Wextra -pedantic\
	$frumul_path/bytecode.cpp\
	$frumul_path/compiler.cpp\
	$frumul_path/exception.cpp\
	$frumul_path/fstring.cpp \
	$frumul_path/mark.cpp\
	$frumul_path/name.cpp\
	$frumul_path/node.cpp\
	$frumul_path/parameters.cpp\
	$frumul_path/parmqueuer.cpp\
	$frumul_path/position.cpp\
	$frumul_path/retchecker.cpp\
	$frumul_path/schildren.cpp\
	$frumul_path/symbol.cpp\
	$frumul_path/symboltab.cpp\
	$frumul_path/tailresult.cpp\
	$frumul_path/util.cpp\
	$frumul_path/value.cpp\
	$frumul_path/vm.cpp\
	$frumul_path/vmtypes.cpp\
	-I$frumul_path/icu/usr/local/include \
	-I$frumul_path \
	-L$icu_path/usr/local/lib/ \
	-Wl,-R$frumul_path/icu/usr/local/lib/ \
	-licuuc -licudata -licuio -licui18n \
	-DRANDOM_VALUE=${RANDOM} \
	&& valgrind ./a.out
