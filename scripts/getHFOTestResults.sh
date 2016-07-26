#!/bin/bash
files=$(ls *.std);
pts=$(for f in $files; do cat $f | grep soc::test | awk -F"meanOutcomeTest" '{print $2}' | awk '{print $1}' | sort -n | tail -n 1; done)
sortedTop=$(echo $pts | tr ' ' '\n' | sort -n)

echo "Proportion of test games with goal scored:"
echo $sortedTop | tr ' ' '\n'
