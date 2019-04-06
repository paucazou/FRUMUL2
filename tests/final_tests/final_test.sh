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
	result=`$ex $file.uu --no-browser 2>&1`
	if [[ $result != `cat $file.result` ]]; then
		print Error in $file.uu
		print Output:
		print $result
	else
		# test memory leak only if no error
		mem_leak_result=`valgrind --leak-check=yes -q $ex $file.uu --no-browser 2>&1`
		if [[ $? != 0 ]]; then
			print Memory leak in $file.uu
			print $mem_leak_result
		else
			print OK
		fi
	fi

done
print Tests finished

