#1/bin/bash
t=3
Mgap=2
files=$(ls sbb*std); for f in $files; do cat $f | grep "tminfo t $t " | sort -nk 19 | tail -n $Mgap | cut -d ' ' -f 7,19 >> scoring_BestFromEachRun.rslt; done
