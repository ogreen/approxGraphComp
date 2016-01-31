#!/bin/bash 

bin="../sv"

file="$1"
# echo "input file is " $file

for i in `seq 9 10`;
do
	NORM_PROB=$i $bin $file
done