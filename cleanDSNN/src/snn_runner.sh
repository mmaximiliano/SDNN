#!/bin/bash

echo "----- Running massive sequence creator -----"

c=1

for fname in "all_nums_20"
do
	for seed in 9
  do
  	echo "Loop: $c/1"
  	echo "Running fname= $fname seed= $seed"
  	python3 main.py -fname $fname -s $seed
  	echo
  	c=$((c+1))
	done
done

echo "Done."