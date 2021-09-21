#!/bin/bash

echo "----- Running massive sequence creator -----"

c=1

for fname in "all_nums" "all_nums_5" "all_nums_20"
do
	for seed in 0 1 2 3 4 5 6 7 8 9
  do
  	echo "Loop: $c/30"
  	echo "Creating p= 1 pf= 3 fname= $fname seed= $seed "
  	python3 wevents_seqgen.py -p 1 -nn 0 2 3 4 5 6 7 8 9 -pf 3 -fname $fname -seed $seed
  	echo
  	c=$((c+1))
	done
done

echo "Done."