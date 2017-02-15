#!/bin/bash
r=$1
id=9016246
t=100
#dir=$2
#t=$3
./genScript.pl sbb 1 $r
#j=0
#MPMvsG
#if [ ! -d "$dir-MPMvsG" ]; then mkdir $dir-MPMvsG; fi
#f="$dir/sbb.$r.std-testTeams.rslt"
#echo "Running test on run $r..."
#while read -r line || [[ -n "$line" ]]; do
#  t=$(echo $line| cut -d ' ' -f 1)
#  id=$(echo $line| cut -d ' ' -f 2)
  #MPMvsG
  java -jar $SBBTDPATH/src/java/MsPacManNew/bin/pacman/MsPacManSimulator-points.jar randomSeed:$r usePoints:false pacManGainsLives:true pacmanLives:3 pacManLevelTimeLimit:3000 pacmanMaxLevel:16 pacmanFatalTimeLimit:false timedPacman:false getRemainingPills:true 1> pacman.$r.$id.std 2> pacman.$r.$id.err &
  sleep 3
  $SBBTDPATH/build/release/cpp/mspacmanSBBAgent/mspacmanSBBAgent -P $id -s $r -p $r -L 2 -C 0 -t $t -l 1 -f 1 1> sbb.$r.$id.std 2> sbb.$r.$id.err &
  #j=$(echo "$j+5" | bc)
  #sleep 3
#done < $f
