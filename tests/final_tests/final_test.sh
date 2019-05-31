#!/bin/zsh
# This script must be called from the frumul root

# some paths
dir_path="./tests/final_tests/"
ex="./out"

# load file names
_raw_file_names=`cat $dir_path/test_files`
file_names=("${(f)_raw_file_names}")

# functions
get_result () {
	$ex $file.uu --no-browser 2>&1
}

get_memory_leak_result () {
	valgrind --leak-check=yes -q $ex $file.uu --no-browser 2>&1
}

check_memory_leak () {
	if [[ $? != 0 ]]; then
		print Memory leak in $file.uu
		print $mem_leak_result
	else
		print OK
	fi
}


# iterates over files
for file in $file_names; do
	file=$dir_path$file
	print Check $file.uu...
	# test the result
	result=`get_result`
	if [[ $result != `cat $file.result` ]]; then
		print Error in $file.uu
		print Output:
		print $result
	else
		# test memory leak only if no error
		mem_leak_result=`get_memory_leak_result`
		check_memory_leak
	fi

done

print Testing some special files...
# stdin
file=$dir_path"include_io_in"
print Check $file.uu...
result=`print message | get_result`
if [[ $result != `cat $file.result` ]]; then
	print Error in $file.uu
	print Output:
	print $result
else
	mem_leak_result=`print message | get_memory_leak_result`
	check_memory_leak
fi

# stderr
file=$dir_path"include_io_err"
print Check $file.uu...
output=`$ex $file.uu --no-browser 2>/dev/null`
error=`$ex $file.uu --no-browser 2>&1 >/dev/null`
if [[ $output != `cat $file.result` ]] ; then
	print Error in $file.uu
	print Output:
	print $output
elif [[ $error != 'Error' ]]; then
	print Error in $file.uu
	print Error:
	print $error
else
	mem_leak_result=`get_memory_leak_result`
	check_memory_leak
fi

print Tests finished

