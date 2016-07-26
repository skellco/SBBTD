#!/bin/bash
rSeed=$1

echo "Starting run $rSeed..."

./genScript.pl sbb 1 $rSeed

sleep 1

#Scoring 4v4
$SBBTDPATH/build/release/cpp/experiments_robocup/exp_robocup -D 0 -T 125 -L 1 -O 5 -R -s $rSeed -A 4 -B 4 -f 17 -Y 1 1> sbb.$rSeed.std 2> sbb.$rSeed.err & #Scoring 4v4
