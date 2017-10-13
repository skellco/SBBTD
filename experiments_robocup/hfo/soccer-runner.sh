#!/bin/bash
rSeed=$1

echo "Starting run $rSeed..."

./genScript.pl sbb 1 $rSeed

sleep 1

#HFO with transfer
$SBBTDPATH/build/release/cpp/experiments_robocup/exp_robocup -m -N -D 1 -T 125 -L 2 -H -t 0 -l 0 -Q 50 -O 5 -R -s $rSeed -A 4 -B 4 -f 17 -Y 1 1> sbb.$rSeed.std 2> sbb.$rSeed.err & 
