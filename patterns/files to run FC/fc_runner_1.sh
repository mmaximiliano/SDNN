#!/bin/bash

echo "----- Running massive sequence FC -----"

c=1


for seed in 0 1 2 3 4 5 6 7 8 9
do
	for weight in 0 3 7
  	do
		echo "Loop: $c/30"
		echo "Creating folder: all_nums seed: $seed neurons: 8 w: $weight"
		python3 train_FC.py -folder "all_nums" -seed $seed -neurons 8 -w $weight
		echo
		c=$((c+1))

	done
done

echo "Done."