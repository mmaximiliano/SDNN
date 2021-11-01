#!/bin/bash

echo "----- Running massive sequence creator -----"

c=1

for c_1 in 30 25 20 15 10 5 0
do
	for c_2 in 0 5 10 15 20 25 30
  do
    for c_3 in 0 5 10 15 20 25 30
    do
  	  echo "Loop: $c/343"
  	  echo "Running c_1= $c_1 c_2= $c_2 c_3= $c_3"
  	  python3 main.py -cuno $c_1 -cdos $c_2 -ctres $c_3
  	  echo
  	  c=$((c+1))
  	done
	done
done

echo "Done."