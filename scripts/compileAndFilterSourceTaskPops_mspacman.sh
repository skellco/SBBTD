#!/bin/bash
Mgap=4
t=3

if [ ! -d "$SBBTDPATH/data_mspacman" ]; then mkdir $SBBTDPATH/data_mspacman; 
else rm -rf $SBBTDPATH/data_mspacman/*
fi

cd $SBBTDPATH/mspacman-ghostscore
$SBBTDPATH/scripts/getGhostscoreBestTeams.sh
cp ghostscore_BestFromEachRun.rslt ../data_mspacman

cd $SBBTDPATH/mspacman-pillscore
$SBBTDPATH/scripts/getPillscoreBestTeams.sh
cp pillscore_BestFromEachRun.rslt $SBBTDPATH/data_mspacman

cd $SBBTDPATH/data_mspacman
cp -r ../mspacman-ghostscore/checkpoints checkpoints-ghostscore
cp -r ../mspacman-pillscore/checkpoints checkpoints-pillscore
../scripts/prepare-single-composit.sh 7100 3 checkpoints-ghostscore ghostscore-composite
../scripts/prepare-single-composit.sh 8100 3 checkpoints-pillscore pillscore-composite
../scripts/compileTransferSourcePolicies.sh $Mgap $t mspacman

cd $SBBTDPATH/runPacman
./cleanup
cp ../data_mspacman/TransferSourcePolicies/* checkpoints
