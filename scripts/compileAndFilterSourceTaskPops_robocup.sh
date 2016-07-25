#!/bin/bash

if [ ! -d "$SBBTDPATH/data_robocup" ]; then mkdir $SBBTDPATH/data_robocup; 
else rm -rf $SBBTDPATH/data_robocup/*
fi

cd $SBBTDPATH/robocup-keepaway
$SBBTDPATH/scripts/getKeepawayBestTeams.sh
cp keepaway_BestFromEachRun.rslt ../data_robocup

cd $SBBTDPATH/robocup-scoring
$SBBTDPATH/scripts/getScoringBestTeams.sh
cp scoring_BestFromEachRun.rslt $SBBTDPATH/data_robocup

cd $SBBTDPATH/data_robocup
cp -r ../robocup-keepaway/checkpoints checkpoints-keepaway
cp -r ../robocup-scoring/checkpoints checkpoints-scoring
../scripts/prepare-single-composit.sh 7100 3 checkpoints-keepaway keepaway-composite
../scripts/prepare-single-composit.sh 8100 3 checkpoints-scoring scoring-composite
../scripts/compileTransferSourcePolicies.sh robocup

cd $SBBTDPATH/runSoccer
./cleanup
cp ../data_robocup/TransferSourcePolicies/* checkpoints
