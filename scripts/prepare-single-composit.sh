#!/bin/bash
baseSeed=$1
t=$2
checkpointDir=$3 
compositDir=$4

#build composit checkpoint ##########################################################################################

files=$(ls $checkpointDir/cp.0.$t.*)
echo "seed:$baseSeed" > tmp
echo "learnerPop:" >> tmp

for f in $files; do cat $f | grep "learner:" >> tmp; done

echo "endLearnerPop" >> tmp
echo "teamPop:" >> tmp

for f in $files; do cat $f | grep "team:" >> tmp; done

echo "endTeamPop" >> tmp

if [ ! -d "$compositDir" ]; then mkdir $compositDir; fi
mv tmp $compositDir/cp.0.$t.-1.$baseSeed.0.rslt
