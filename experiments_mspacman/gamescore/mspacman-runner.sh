#!/bin/bash
rSeed=$1

echo "Running seed/port $rSeed..."

./genScript.pl sbb 1 $rSeed

# start simulator #############################################

#MPMvsG
java -jar $SBBTDPATH/src/java/MsPacManNew/bin/pacman/MsPacManSimulator-points.jar randomSeed:$rSeed usePoints:false pacManGainsLives:true pacmanLives:3 pacManLevelTimeLimit:3000 pacmanMaxLevel:16 pacmanFatalTimeLimit:false getRemainingPills:true 1> pacman.$rSeed.std 2> pacman.$rSeed.err &

sleep 3

# start sbb ###################################################

#Gamescore with transfer
$SBBTDPATH/build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -s $rSeed -p $rSeed -T 3 -L 2 -O 5 -f 1 -H -C 0 -t 0 -l 0  1> sbb.$rSeed.std 2> sbb.$rSeed.err & 

