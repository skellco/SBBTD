#!/bin/bash

r=$1
firstRun=$2
lastRun=$3
numEval=$4

if [ $r -eq 1 ]; then rw="gameScore"; fi
if [ $r -eq 2 ]; then rw="pillScore"; fi
if [ $r -eq 3 ]; then rw="ghostScore"; fi

#means
sumAll=0
for run in `seq $firstRun 100 $lastRun`; do
   maxMean=0
   fBest=""
   files=$(ls sbb.$run.*.std)
   for f in $files; do
      sum=$(cat $f | grep runEval | awk -F"$rw" '{print $2}' | awk '{print $1}' | tr '\n' '+')
      mean=$(echo "scale=4; ($sum 0)/$numEval" | bc)
      if (( $(echo "$mean > $maxMean" | bc -l) )); then maxMean=$mean; fBest=$f; fi
   done
   sumAll=$(echo "scale=4; $sumAll + $maxMean" | bc)
   echo "Run $fBest Gamescore $maxMean"
done
meanAll=$(echo "scale=4; $sumAll/10" | bc)
echo "Overall mean: $meanAll"

