#!/bin/bash
rSeed=$1
#Modes
# 0:train ghostscore
# 1:train pillscore
# 2:train gamescore with transfer
# 3:replay policy
mode=$2 

./genScript.pl sbb 1 $rSeed

# start simulator #############################################

if [ $mode -lt 3 ]; then
#train
java -jar MsPacManSimulator-points.jar randomSeed:$rSeed usePoints:false pacManLevelTimeLimit:8000 1> pacman.$rSeed.std 2> pacman.$rSeed.err &

else
#MPMvsG
java -jar MsPacManSimulator-points.jar randomSeed:$rSeed usePoints:false pacManGainsLives:true pacmanLives:3 pacManLevelTimeLimit:3000 pacmanMaxLevel:16 pacmanFatalTimeLimit:false getRemainingPills:true 1> pacman.$rSeed.std 2> pacman.$rSeed.err &
fi

sleep 3

# start sbb ###################################################

if [ $mode -eq 0 ]; then
#from scratch ghostscore task
../build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -s $rSeed -p $rSeed -O 5 -f 2 1> sbb.$rSeed.std 2> sbb.$rSeed.err &


elif [ $mode -eq 1 ]; then
#from scratch pillscore task
../build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -s $rSeed -p $rSeed -O 5 -f 1 1> sbb.$rSeed.std 2> sbb.$rSeed.err &

elif [ $mode -eq 2 ]; then
#pickup with new level
../build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -s $r -p $rSeed -f 0 -O 5 -H -C 0 -t 3 -l 0  1> sbb.$rSeed.std 2> sbb.$rSeed.err &

elif [ $mode -eq 3 ]; then
#replay
../build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -V -P 9016246 -1 -s $rSeed -p $rSeed -C 0 -T 100 -L 1 -f 1 1> sbb.$rSeed.std 2> sbb.$rSeed.err &
fi
