#!/bin/bash
r=$1
./genScript.pl sbb 1 $r

# start simulator #############################################

#train
#java -jar MsPacManSimulator-points.jar randomSeed:$r usePoints:false pacManLevelTimeLimit:8000 1> pacman.$r.std 2> pacman.$r.err &
#MPMvsG
java -jar MsPacManSimulator-points.jar randomSeed:$r usePoints:false pacManGainsLives:true pacmanLives:3 pacManLevelTimeLimit:3000 pacmanMaxLevel:16 pacmanFatalTimeLimit:false getRemainingPills:true 1> pacman.$r.std 2> pacman.$r.err &

sleep 3

# start sbb ###################################################

#from scratch
../build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -V -s $r -p $r -O 5 -f 0 1> sbb.$r.std 2> sbb.$r.err &

#continuation without new level
#../build/release/mspacmanSBBAgent/mspacmanSBBAgent -V -s $r -O 5 -C 0 -T 110 -L 0 -t 110 -l 0 1> sbb.$r.std 2> sbb.$r.err &

#continuation with new level
#../build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -s $r -O 5 -C 0 -H -f 1 -T 201 -L 0 -t 201 -l 0 1> sbb.$r.std 2> sbb.$r.err &

#replay
#../build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -V -P 9016246 -1 -s $r -p $r -C 0 -T 100 -L 1 -f 1 1> sbb.$r.std 2> sbb.$r.err &

