#!/bin/zsh
# This script must be called from the frumul root

# some paths
dir_path="./tests/final_tests/"
ex="./out"

# load file names
_raw_file_names=`cat $dir_path/test_files`
file_names=("${(f)_raw_file_names}")

# iterates over files
for file in $file_names; do
	file=$dir_path$file
	print Check $file.uu...
	# test the result
	result=`$ex $file.uu 2>&1`
	if [[ $result != `cat $file.result` ]]; then
		print Error in $file.uu
		print Output:
		print $result
	fi

	# test memory leak
	valgrind --leak-check=yes -q $ex $file.uu 2>/dev/null 1>&2
	if [[ $? != 0 ]]; then
		print Memory leak in $file.uu
		valgrind --leak-check=yes $ex $file.uu
	fi

done
print Tests finished

