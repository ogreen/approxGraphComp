#!/bin/bash 

bin="../sv"

file="$1"
# echo "input file is " $file

for i in `seq 6 2 18`;
do
	NORM_PROB=$i $bin $file
	# echo Ran i $bin $file successfully
done