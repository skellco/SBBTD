#!/bin/bash
rSeed=$1

echo "Running seed/port $rSeed..."

./genScript.pl sbb 1 $rSeed

# start simulator #############################################

#train
java -jar $SBBTDPATH/src/java/MsPacManNew/bin/pacman/MsPacManSimulator-points.jar randomSeed:$rSeed usePoints:false pacManLevelTimeLimit:8000 1> pacman.$rSeed.std 2> pacman.$rSeed.err &

sleep 3

#Pillscore task
$SBBTDPATH/build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -s $rSeed -p $rSeed -T 3 -L 1 -O 5 -f 2 1> sbb.$rSeed.std 2> sbb.$rSeed.err & 