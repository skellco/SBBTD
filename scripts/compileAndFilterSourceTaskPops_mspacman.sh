#!/bin/bash
cd $SBBTDPATH

tA=$(cat $SBBTDPATH/experiments_mspacman/ghostscore/mspacman-runner.sh | grep "mspacmanSBBAgent" | awk -F"-T" '{print $2}' | awk '{print $1}' | tr -d ';')
tB=$(cat $SBBTDPATH/experiments_mspacman/pillscore/mspacman-runner.sh | grep "mspacmanSBBAgent" | awk -F"-T" '{print $2}' | awk '{print $1}' | tr -d ';')

if [ ! -d "$SBBTDPATH/data_mspacman" ]; then mkdir $SBBTDPATH/data_mspacman; 
else rm -rf $SBBTDPATH/data_mspacman/*
fi

cd $SBBTDPATH/experiments_mspacman/ghostscore
$SBBTDPATH/scripts/getGhostscoreBestTeams.sh
cp ghostscore_BestFromEachRun.rslt $SBBTDPATH/data_mspacman

cd $SBBTDPATH/experiments_mspacman/pillscore
$SBBTDPATH/scripts/getPillscoreBestTeams.sh
cp pillscore_BestFromEachRun.rslt $SBBTDPATH/data_mspacman

cd $SBBTDPATH/data_mspacman
cp -r $SBBTDPATH/experiments_mspacman/ghostscore/checkpoints checkpoints-ghostscore
cp -r $SBBTDPATH/experiments_mspacman/pillscore/checkpoints checkpoints-pillscore
$SBBTDPATH/scripts/prepare-single-composit.sh 7100 $tA checkpoints-ghostscore ghostscore-composite
$SBBTDPATH/scripts/prepare-single-composit.sh 8100 $tB checkpoints-pillscore pillscore-composite
$SBBTDPATH/scripts/compileTransferSourcePolicies.sh mspacman

cd $SBBTDPATH/experiments_mspacman/gamescore
./cleanup
cp $SBBTDPATH/data_mspacman/TransferSourcePolicies/* checkpoints
