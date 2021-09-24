#!/bin/bash

echo "----- Running massive sequence creator -----"

c=1

for fname in "all_nums_5" "all_nums_20"
do
	for seed in 0 1 2 3 4 5 6 7 8 9
  do
  	echo "Loop: $c/20"
  	echo "Running fname= $fname seed= $seed"
  	python3 main.py -fname $fname -s $seed
  	echo
  	c=$((c+1))
	done
done

echo "Done."