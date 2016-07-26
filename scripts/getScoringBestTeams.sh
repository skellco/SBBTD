#!/bin/bash
Mgap=$(cat genScript-scoring.pl | grep "my \$Mgap =" | awk -F"=" '{print $2}' | awk '{print $1}' | tr -d ';')
t=$(cat soccer-runner.sh | grep "#Scoring 4v4" | awk -F"-T" '{print $2}' | awk '{print $1}' | tr -d ';')
files=$(ls sbb*std); for f in $files; do cat $f | grep "tminfo t $t " | sort -nk 19 | tail -n $Mgap | cut -d ' ' -f 7,19 >> scoring_BestFromEachRun.rslt; done
