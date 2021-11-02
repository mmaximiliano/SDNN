#!/bin/bash

echo "----- Running massive sequence creator -----"

c=1

for c_1 in 15
do
	for c_2 in 0 5 10 15 20 25 30
  do
    for c_3 in 0 5 10 15 20 25 30
    do
  	  echo "Loop: $c/49"
  	  echo "Running c_1= $c_1 c_2= $c_2 c_3= $c_3"
  	  python3 main-delay.py -c1 $c_1 -c2 $c_2 -c3 $c_3
  	  echo
  	  c=$((c+1))
  	done
	done
done

echo "Done."