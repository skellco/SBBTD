#!/bin/bash
baseSeed=9100
checkpointDir="gamescore-ar-div-001_t201_checkpoints"
compositDir="gamescore-ar-div-001_t201-single-composit"

#build composit checkpoint ##########################################################################################

files=$(ls $checkpointDir)
echo "seed:$baseSeed" > tmp
echo "learnerPop:" >> tmp

for f in $files; do cat $checkpointDir/$f | grep "learner:" >> tmp; done

echo "endLearnerPop" >> tmp
echo "teamPop:" >> tmp

for f in $files; do cat $checkpointDir/$f | grep "team:" >> tmp; done

echo "endTeamPop" >> tmp

if [ ! -d "$compositDir" ]; then mkdir $compositDir; fi
mv tmp $compositDir/cp.0.201.-1.$baseSeed.0.rslt
