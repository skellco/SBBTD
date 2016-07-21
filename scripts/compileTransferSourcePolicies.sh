#!/bin/bash
Mgap=$1
t=$2
compositeDirA="ghostscore-composite"
teamsA="ghostscore_BestFromEachRun.rslt"
compositeDirB="pillscore-composite"
teamsB="pillscore_BestFromEachRun.rslt"
################################################################################################################
#compile teamsA

cp $compositeDirA/cp.*.rslt composit-1

#get top teams
topTeams=$(cat $teamsA | sort -n -k2 | cut -d ' ' -f 1 | tail -n $Mgap)
echo "" > composit-teams
for tm in $topTeams
  do
    cat composit-1 | grep team:$tm >> composit-teams
  done

#remove all teams
sed -i '/team:/d' ./composit-1
sed -i '/endTeamPop/d' ./composit-1

#put selected teams back
cat composit-teams >> composit-1
rm composit-teams
echo "endTeamPop" >> composit-1
if [ ! -d "TransferSourcePolicies" ]; then mkdir TransferSourcePolicies; fi
mv composit-1 TransferSourcePolicies/compA
################################################################################################################
#compile teamsB
cp $compositeDirB/cp.*.rslt composit-1

#get top teams
topTeams=$(cat $teamsB | sort -n -k2 | cut -d ' ' -f 1 | tail -n $Mgap)
echo "" > composit-teams
for tm in $topTeams
  do
    cat composit-1 | grep team:$tm >> composit-teams
  done

#remove all teams
sed -i '/team:/d' ./composit-1
sed -i '/endTeamPop/d' ./composit-1

#put selected teams back
cat composit-teams >> composit-1
rm composit-teams
echo "endTeamPop" >> composit-1

#combine checkpoints containing teamsA and teamsB 
../scripts/combineCheckpoints.sh composit-1  TransferSourcePolicies/compA  composit-2
mv composit-2 TransferSourcePolicies/cp.0.$t.-1.7100.0.rslt
sed -i '/seed:/ c\seed:7100' TransferSourcePolicies/cp.0.$t.-1.7100.0.rslt #final checkpoint containing all teams
rm composit-1
rm TransferSourcePolicies/compA

# copy final checkpoint with 10 different seeds (for 10 runs) ##################################################
for i in `seq 7200 100 8000`; do
  cp TransferSourcePolicies/cp.0.$t.-1.7100.0.rslt TransferSourcePolicies/cp.0.$t.-1.$i.0.rslt
  sed -i "s/seed:7100/seed:$i/g" TransferSourcePolicies/cp.0.$t.-1.$i.0.rslt
done

