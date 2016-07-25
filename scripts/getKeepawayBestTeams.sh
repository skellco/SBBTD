#1/bin/bash
t=3
Mgap=4
files=$(ls sbb*std); for f in $files; do cat $f | grep "tminfo t $t " | sort -nk 23 | tail -n $Mgap | cut -d ' ' -f 7,23 >> ghostscore_BestFromEachRun.rslt; done
