#!/bin/bash

echo "----- Running massive sequence Single Neuron -----"

c=1


for seed in 0 1 2 3 4 5 6 7 8 9
do
	for weight in "0-0" "0-3" "0-7"
  	do
  		for umbral in "1-0" "1-5" "2-0" "2-5" "3-0" "3-5" "4-0" "4-5"
  		do
			echo "Loop: $c/240"
			echo "Creating folder: all_nums_5 seed: $seed neurons: 16 w: $weight umbral: $umbral"
			python3 train_singleNeuron.py -folder "all_nums_5" -seed $seed -neurons "16" -w $weight -umbral $umbral
			echo
			c=$((c+1))
		done
	done
done

echo "Done."