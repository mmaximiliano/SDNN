#!/bin/bash

echo "----- Running massive sequence creator -----"

c=1

for fname in "p_18_5"
do
	for seed in 0 1 2 3 4
  do
  	echo "Loop: $c/1"
  	echo "Running fname= $fname seed= $seed"
  	python3 main.py -fname $fname -s $seed
  	echo
  	c=$((c+1))
	done
done

echo "Done."