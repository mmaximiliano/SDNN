#!/bin/bash

echo "----- Running massive sequence creator -----"

c=1

for fname in "p_18"
do
	for seed in 5 6 7 8 9
  do
  	echo "Loop: $c/1"
  	echo "Running fname= $fname seed= $seed"
  	python3 main-delay.py -fname $fname -s $seed
  	echo
  	c=$((c+1))
	done
done

echo "Done."