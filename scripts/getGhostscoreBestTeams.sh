#1/bin/bash
Mgap=$(cat genScript.pl | grep "my \$Mgap =" | awk -F"=" '{print $2}' | awk '{print $1}' | tr -d ';')
t=$(cat mspacman-runner.sh | grep "mspacmanSBBAgent" | awk -F"-T" '{print $2}' | awk '{print $1}' | tr -d ';')
files=$(ls sbb*std); for f in $files; do cat $f | grep "tminfo t $t " | sort -nk 23 | tail -n $Mgap | cut -d ' ' -f 7,23 >> ghostscore_BestFromEachRun.rslt; done
