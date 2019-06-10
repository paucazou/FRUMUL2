#!/bin/zsh
bstrlib=frumul/bstrlib
odir=odir
_compile_base=( -std=c++17 -Wextra -Wall -pedantic -pedantic-errors -Wsign-conversion )

_sourcefile=`readlink -f ${${(%):-%N}}`
export FRUMUL_STDLIB=${_sourcefile%/*}"/stdlib/"
print Variable environment set: '$FRUMUL_STDLIB'=$FRUMUL_STDLIB

_manage_parms () {
	# modifies a parameter named NAMED_ARGS, for main parameters
	# and POS_ARGS, for other parameters
	
	# default parameters
	NAMED_ARGS[compiler]="g++"

	for i in "$@"
	do
		case $i in
			-c|--compiler)
				NAMED_ARGS[compiler]=$2
				print $NAMED_ARGS[compiler]
				shift
				shift
				;;
			-d|--debug)
				POS_ARGS+=(-D DEBUG -g)
				print DEBUG ON
				shift
				;;

			-f|--file)
				NAMED_ARGS[file]=$2;
				shift
				shift
				;;
			*)
				POS_ARGS+=$1
				shift
				;;
		esac
	done
}

compile () { # deprecated
	if [[ $1 == 'clang' ]]; then
		compiler=$1++
		shift
	else
		compiler='g++'
	fi
	$compiler -std=c++17\
		-Wextra -Wall -pedantic\
		-pedantic-errors\
		-Wfatal-errors\
		-Wsign-conversion\
		-D DEBUG\
		-D CXXOPTS_USE_UNICODE\
		-g\
		tests/astbrowser.cpp\
		tests/tests.cpp\
		frumul/alias.cpp\
		frumul/argcollector.cpp\
		frumul/bytecode.cpp\
		frumul/compiler.cpp\
		frumul/exception.cpp\
		frumul/fdeclaration.cpp\
		frumul/fstring.cpp\
		frumul/hinterpreter.cpp\
		frumul/lexer.cpp\
		frumul/mark.cpp\
		frumul/name.cpp\
		frumul/node.cpp\
		frumul/parameters.cpp\
		frumul/parmqueuer.cpp\
		frumul/parser.cpp\
		frumul/position.cpp\
		frumul/retchecker.cpp\
		frumul/schildren.cpp\
		frumul/symbol.cpp\
		frumul/symboltab.cpp\
		frumul/tailresult.cpp\
		frumul/textto.cpp\
		frumul/transpiler.cpp\
		frumul/util.cpp\
		frumul/value.cpp\
		frumul/vm.cpp\
		frumul/vmtypes.cpp\
		frumul/warning.cpp\
		-o out\
		$odir/token.o\
		/usr/lib/gcc/x86_64-linux-gnu/6/libstdc++fs.a\
		-isystem frumul/icu/usr/local/include\
		-isystem frumul/cxxopts/include\
		-licuuc -licudata -licuio -licui18n \
		-Lfrumul/icu/usr/local/lib \
		-Wl,-Rfrumul/icu/usr/local/lib\
		$@
}

build_main_lib () {
	typeset -A NAMED_ARGS 
	POS_ARGS=()

	_manage_parms $@
	compiler=$NAMED_ARGS[compiler]
	unset "NAMED_ARGS[compiler]"

	$compiler $NAMED_ARGS $_compile_base \
		-Wfatal-errors \
		-shared \
		-o libfrumul.so\
		-fPIC\
		frumul/alias.cpp\
		frumul/argcollector.cpp\
		frumul/bytecode.cpp\
		frumul/compiler.cpp\
		frumul/exception.cpp\
		frumul/dynloader.cpp\
		frumul/fdeclaration.cpp\
		frumul/fstring.cpp\
		frumul/hinterpreter.cpp\
		frumul/lexer.cpp\
		frumul/mark.cpp\
		frumul/name.cpp\
		frumul/node.cpp\
		frumul/parameters.cpp\
		frumul/parmqueuer.cpp\
		frumul/parser.cpp\
		frumul/position.cpp\
		frumul/retchecker.cpp\
		frumul/schildren.cpp\
		frumul/stackvar.cpp\
		frumul/symbol.cpp\
		frumul/symboltab.cpp\
		frumul/tailresult.cpp\
		frumul/textto.cpp\
		frumul/token.cpp\
		frumul/transpiler.cpp\
		frumul/util.cpp\
		frumul/value.cpp\
		frumul/vm.cpp\
		frumul/vmtypes.cpp\
		frumul/warning.cpp\
		/usr/lib/gcc/x86_64-linux-gnu/8/libstdc++fs.a\
		-isystem frumul/icu/usr/local/include\
		-isystem frumul/cxxopts/include\
		-licuuc -licudata -licuio -licui18n \
		-Lfrumul/icu/usr/local/lib \
		-Wl,-Rfrumul/icu/usr/local/lib \
		$POS_ARGS

}

_build_main () {
	typeset -A NAMED_ARGS
	POS_ARGS=()
	_manage_parms $@
	compiler=$NAMED_ARGS[compiler]
	unset "NAMED_ARGS[compiler]"

	$compiler $_compile_base \
		-D CXXOPTS_USE_UNICODE\
		-Wfatal-errors \
		-L./ \
		-Wl,-rpath=./ \
		$files \
		-lfrumul \
		-ldl \
		-o $output_file\
		/usr/lib/gcc/x86_64-linux-gnu/8/libstdc++fs.a\
		-isystem frumul/icu/usr/local/include\
		-isystem frumul/cxxopts/include\
		-isystem frumul/include \
		-licuuc -licudata -licuio -licui18n \
		-Lfrumul/icu/usr/local/lib \
		-Wl,-Rfrumul/icu/usr/local/lib \
		$POS_ARGS
	}

build_main () {
	files=(frumul/main.cpp frumul/cmdline.cpp)
	output_file="FRUMUL"
	_build_main $@
}

build_tests () {
	files=(tests/astbrowser.cpp tests/tests.cpp)
	output_file="out"
	_build_main $@
}

build_lib () {
	typeset -A NAMED_ARGS
	POS_ARGS=()
	_manage_parms $@
	compiler=$NAMED_ARGS[compiler]
	unset "NAMED_ARGS[compiler]"
	input_file="frumul/stdlib/$NAMED_ARGS[file].cpp"
	output_file="${FRUMUL_STDLIB}${NAMED_ARGS[file]}.hb"
	print File: $input_file

	$compiler $_compile_base \
		-shared \
		-fPIC \
		-L. \
		-Wl,-rpath=./ \
		$input_file \
		-o $output_file \
		-lfrumul \
		-I frumul/ \
		-isystem frumul/icu/usr/local/include\
		-isystem frumul/cxxopts/include\
		-licuuc -licudata -licuio -licui18n \
		-Lfrumul/icu/usr/local/lib \
		-Wl,-Rfrumul/icu/usr/local/lib \
		$POS_ARGS
}



objectify () {
	typeset -A NAMED_ARGS
	POS_ARGS=()
	_manage_parms $@
	compiler=$NAMED_ARGS[compiler]
	unset "NAMED_ARGS[compiler]"
	print File: $POS_ARGS[1]

	cd $odir
	$compiler $NAMED_ARGS $_compile_base \
		-I ../frumul \
		-isystem ../frumul/icu/usr/local/include\
		-isystem ../frumul/cxxopts/include\
		-isystem ../frumul/include\
		-D DEBUG\
		-g\
		-c ../$POS_ARGS[1]
	popd
}


alias check_frumul="cppcheck ./frumul/ --enable=all --inconclusive --force --std=c++11 -i $bstrlib"
#alias valgrind="valgrind --leak-check=yes --leak-check=full --show-leak-kinds=all"
alias valgrind="valgrind --leak-check=yes"
alias final_test="./tests/final_tests/final_test.sh"
	

