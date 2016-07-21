#1/bin/bash
t=100
Mgap=90
files=$(ls sbb*std); for f in $files; do cat $f | grep "tminfo t $t " | sort -nk 21 | tail -n $Mgap | cut -d ' ' -f 7,21 >> pillscore_BestFromEachRun.rslt; done
