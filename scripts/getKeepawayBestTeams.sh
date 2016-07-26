#1/bin/bash
Mgap=$(cat genScript.pl | grep "my \$Mgap =" | awk -F"=" '{print $2}' | awk '{print $1}' | tr -d ';')
t=$(cat soccer-runner.sh | grep "exp_robocup" | awk -F"-T" '{print $2}' | awk '{print $1}' | tr -d ';')
files=$(ls sbb*std); for f in $files; do cat $f | grep "tminfo t $t " | sort -nk 19 | tail -n $Mgap | cut -d ' ' -f 7,19 >> keepaway_BestFromEachRun.rslt; done
