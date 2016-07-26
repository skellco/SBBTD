#!/bin/bash
cd $SBBTDPATH

tA=$(cat $SBBTDPATH/experiments_robocup/keepaway/soccer-runner.sh | grep "exp_robocup" | awk -F"-T" '{print $2}' | awk '{print $1}' | tr -d ';')
tB=$(cat $SBBTDPATH/experiments_robocup/scoring/soccer-runner.sh | grep "exp_robocup" | awk -F"-T" '{print $2}' | awk '{print $1}' | tr -d ';')

if [ ! -d "$SBBTDPATH/data_robocup" ]; then mkdir $SBBTDPATH/data_robocup; 
else rm -rf $SBBTDPATH/data_robocup/*
fi

cd $SBBTDPATH/experiments_robocup/keepaway
$SBBTDPATH/scripts/getKeepawayBestTeams.sh
cp keepaway_BestFromEachRun.rslt $SBBTDPATH/data_robocup

cd $SBBTDPATH/experiments_robocup/scoring
$SBBTDPATH/scripts/getScoringBestTeams.sh
cp scoring_BestFromEachRun.rslt $SBBTDPATH/data_robocup

cd $SBBTDPATH/data_robocup
cp -r ../experiments_robocup/keepaway/checkpoints checkpoints-keepaway
cp -r ../experiments_robocup/scoring/checkpoints checkpoints-scoring
../scripts/prepare-single-composit.sh 7100 $tA checkpoints-keepaway keepaway-composite
../scripts/prepare-single-composit.sh 8100 $tB checkpoints-scoring scoring-composite
../scripts/compileTransferSourcePolicies.sh robocup

cd $SBBTDPATH/experiments_robocup/hfo
./cleanup
cp $SBBTDPATH/data_robocup/TransferSourcePolicies/* checkpoints
