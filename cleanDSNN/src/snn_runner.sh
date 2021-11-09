#!/bin/bash

echo "----- Running massive sequence creator -----"

c=1

for fname in "all_nums"
do
	for seed in 0 7 9
  do
  	echo "Loop: $c/30"
  	echo "Running fname= $fname seed= $seed"
  	python3 main.py -fname $fname -s $seed
  	echo
  	c=$((c+1))
	done
done

echo "Done."