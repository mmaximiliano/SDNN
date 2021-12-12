#!/bin/bash

echo "----- Running massive sequence creator -----"

c=1

for fname in "p_18" "p_18_5" "p_18_20"
do
	for seed in 0 1 2 3 4 5 6 7 8 9
  do
  	echo "Loop: $c/30"
  	echo "Creating p= 1,8 pf= 5 fname= $fname seed= $seed "
  	python3 massive_seqgen.py -p 1 8 -nn 0 2 3 4 5 6 7 9 -pf 5 -fname $fname -seed $seed
  	echo
  	c=$((c+1))
	done
done

echo "Done."