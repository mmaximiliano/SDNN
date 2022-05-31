#!/bin/bash

echo "----- Running massive pattern runner -----"

c=1

for th in 500 600 650 700 750
do
  echo "Loop: $c/1"
  echo "Running th= $th "
  python3 pattern-runner.py -th $th
  echo
  c=$((c+1))
done

echo "Done."