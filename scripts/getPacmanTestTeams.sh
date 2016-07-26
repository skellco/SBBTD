#!/bin/bash
generation=$1
samples=3

if [ ! -d "GamescoreTestTeams" ]; then mkdir GamescoreTestTeams; fi

files=$(ls sbb*std)
for f in $files; do
 echo > $f-tmp
 for t in `seq $generation -1 $(echo "$generation-$samples" | bc)`; do cat $f | grep "tminfo t $t" | sort -nk  19 | cut -d ' ' -f 3,7,19 >> $f-tmp; done
 cat $f-tmp | sort -unk 3 | tail -n $samples | sort -rk3 > GamescoreTestTeams/$f-testTeams.rslt
 rm $f-tmp
done

