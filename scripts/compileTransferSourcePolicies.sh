#!/bin/bash
divCode="nodiv"
################################################################################################################
#compile ghostscrore teams
cp ghostscore-ar-$divCode-001_t100-single-composit/cp.0.100.-1.7100.0.rslt composit-1
#get ghostscore top teams
topTeams=$(cat ghostscore-ar-$divCode-001_t100_best90FromEachRun.rslt | sort -n -k2 | cut -d ' ' -f 1 | tail -n 90)
echo "" > composit-teams
for t in $topTeams
  do
    cat composit-1 | grep team:$t >> composit-teams
  done
#remove all teams
sed -i '/team:/d' ./composit-1
sed -i '/endTeamPop/d' ./composit-1
#put pareto teams back
cat composit-teams >> composit-1
rm composit-teams
echo "endTeamPop" >> composit-1
if [ ! -d "TransferSourcePolicies-$divCode" ]; then mkdir TransferSourcePolicies-$divCode; fi
mv composit-1 TransferSourcePolicies-$divCode/cp.0.100.-1.7100.0.rslt
################################################################################################################
#compile pillscrore teams
cp pillscore-ar-$divCode-001_t100-single-composit/cp.0.100.-1.8100.0.rslt composit-1
#get ghostscore top teams
topTeams=$(cat pillscore-ar-$divCode-001_t100_best90FromEachRun.rslt | sort -n -k2 | cut -d ' ' -f 1 | tail -n 90)
echo "" > composit-teams
for t in $topTeams
  do
    cat composit-1 | grep team:$t >> composit-teams
  done
#remove all teams
sed -i '/team:/d' ./composit-1
sed -i '/endTeamPop/d' ./composit-1
#put pareto teams back
cat composit-teams >> composit-1
rm composit-teams
echo "endTeamPop" >> composit-1

#combine ghostscore and pillscore top teams
combineCheckpoints.sh composit-1  TransferSourcePolicies-$divCode/cp.0.100.-1.7100.0.rslt  composit-2
mv composit-2 TransferSourcePolicies-$divCode/cp.0.100.-1.7100.0.rslt
sed -i 's/seed:8100/seed:7100/g' TransferSourcePolicies-$divCode/cp.0.100.-1.7100.0.rslt
rm composit-1

# compile checkpoints for 10 runs ###################################################################################
for i in `seq 7200 100 8000`; do
  cp TransferSourcePolicies-$divCode/cp.0.100.-1.7100.0.rslt TransferSourcePolicies-$divCode/cp.0.100.-1.$i.0.rslt
  sed -i "s/seed:7100/seed:$i/g" TransferSourcePolicies-$divCode/cp.0.100.-1.$i.0.rslt
done

