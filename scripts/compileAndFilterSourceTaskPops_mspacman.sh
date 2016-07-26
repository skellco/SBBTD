#!/bin/bash
tA=$(cat $SBBTDPATH/mspacman-ghostscore/soccer-runner.sh | grep "#Ghostscore" | awk -F"-T" '{print $2}' | awk '{print $1}' | tr -d ';')
tB=$(cat $SBBTDPATH/mspacman-pillscore/soccer-runner.sh | grep "#Pillscore" | awk -F"-T" '{print $2}' | awk '{print $1}' | tr -d ';')

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
../scripts/prepare-single-composit.sh 7100 $tA checkpoints-ghostscore ghostscore-composite
../scripts/prepare-single-composit.sh 8100 $tB checkpoints-pillscore pillscore-composite
../scripts/compileTransferSourcePolicies.sh mspacman

cd $SBBTDPATH/runPacman
./cleanup
cp ../data_mspacman/TransferSourcePolicies/* checkpoints
