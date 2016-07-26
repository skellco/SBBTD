#!/bin/bash
rSeed=$1

echo "Starting run $rSeed..."

./genScript.pl sbb 1 $rSeed

sleep 1

#Half Field Keepaway 4v3
$SBBTDPATH/build/release/cpp/experiments_robocup/exp_robocup -T 125 -L 1 -O 5 -R -s $rSeed -A 4 -B 3 -f 11 -Y 3 1> sbb.$rSeed.std  2> sbb.$rSeed.err & 
