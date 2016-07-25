#!/bin/bash
if [ ! -d "$SBBTDPATH/data" ]; then mkdir $SBBTDPATH/data; 
else rm -rf $SBBTDPATH/data/*
fi

cd $SBBTDPATH/mspacman-ghostscore
$SBBTDPATH/scripts/getGhostscoreBestTeams.sh
cp ghostscore_BestFromEachRun.rslt ../data

cd $SBBTDPATH/mspacman-pillscore
$SBBTDPATH/scripts/getPillscoreBestTeams.sh
cp pillscore_BestFromEachRun.rslt $SBBTDPATH/data

cd $SBBTDPATH/data
cp -r ../mspacman-ghostscore/checkpoints checkpoints-ghostscore
cp -r ../mspacman-pillscore/checkpoints checkpoints-pillscore
../scripts/prepare-single-composit.sh 7100 3 checkpoints-ghostscore ghostscore-composite
../scripts/prepare-single-composit.sh 8100 3 checkpoints-pillscore pillscore-composite
../scripts/compileTransferSourcePolicies.sh 4 3

cd $SBBTDPATH/runPacman
./cleanup
cp ../data/TransferSourcePolicies/* checkpoints
