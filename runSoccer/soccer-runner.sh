#!/bin/bash
rSeed=$1
#Modes
# 0:train keepaway
# 1:train scoring
mode=$2

echo "Starting run $rSeed..."

if [ $mode -eq 0  ]; then
./genScript.pl sbb 1 $rSeed
elif [ $mode -eq 1  ]; then
./genScript-scoring.pl sbb 1 $rSeed
fi

sleep 1

## Sarsa ####################################################################

#half-field keepaway 4v3
#../build/release/experiments_robocup/sbbsoc -R -s $r -A 4 -B 3 -f 11 -Y 3 -S 20000 1> sarsa.$r.std  2> sarsa.$r.err &

#half-field keepaway 4v3 replay
#../build/release/experiments_robocup/sbbsoc -R -s $r -e 0 -W 20000 -A 4 -B 3 -f 11 -Y 3 -S 1000 1> sarsa.$r.replay.std  2> sarsa.$r.replay.err &

#half-field 4v4
#../build/release/experiments_robocup/sbbsoc -s $r -R -w 20000 -A 4 -B 4 -f 17 -Y 1 -S 20000 1> sarsa.$r.replay.std 2> sarsa.$r.replay.err &

#half-field 4v4 replay
#../build/release/experiments_robocup/sbbsoc -m -N -R -s $r -e 0 -w 20000 -A 4 -B 4 -f 17 -Y 1 -S 1000 1> sarsa.$r.replay.std 2> sarsa.$r.replay.err &

## SBB ######################################################################
if [ $mode -eq 0  ]; then
#Half Field keepaway 4v3
../build/release/cpp/experiments_robocup/exp_robocup -T 1 -L 1 -O 5 -R -s $rSeed -A 4 -B 3 -f 11 -Y 3 1> sbb.$rSeed.std  2> sbb.$rSeed.err &
elif [ $mode -eq 1  ]; then
#Scoring 4v4 
../build/release/cpp/experiments_robocup/exp_robocup -D 0 -T 3 -L 1 -O 5 -R -s $rSeed -A 4 -B 4 -f 17 -Y 1 1> sbb.$rSeed.std 2> sbb.$rSeed.err &
fi



#half-field keepaway 4v3 replay
#../build/release/experiments_robocup/sbbsoc -R -s $r -p 2 -t 125 -l 0 -i 1 -g 100 -A 4 -B 3 -f 11 -Y 3  > sbb.$r.std &

#half-field keepaway 4v3 from checkpoint
#../build/release/experiments_robocup/sbbsoc -H -Q 15 -O 5 -R -s $r -C 0 -T 125 -L 0 -t 125 -l 0 -A 4 -B 3 -f 11 -Y 3 1> sbb.$r.std 2> sbb.$r.err &

#half-field keepaway 4v3 from checkpoint for re-validation
#../build/release/experiments_robocup/sbbsoc -R -s $r -C 2 -T 0 -L 1 -t 0 -l 1 -A 4 -B 3 -f 11 -Y 3 1> sbb.$r.std 2> sbb.$r.err &

#half-field 4v4 from checkpoint for re-validation
#../build/release/experiments_robocup/sbbsoc -s $r -C 1 -T 425 -L 0 -t 425 -l 0 -A 4 -B 4 -f 17 -Y 1 1> sbb.$r.std 2> sbb.$r.err &

#split-level transfer
#../build/release/experiments_robocup/sbbsoc -R -s $r -C 2 -g 10 -T 250 -L 0 -t 250 -l 0 -A 4 -B 4 -f 17 -Y 1 1> sbb.$r.std 2> sbb.$r.err & 

#half-field 4v4 
#../build/release/experiments_robocup/sbbsoc -R -s $r -A 4 -B 4 -f 17 -Y 1 -X 0 1> sbb.$r.std 2> sbb.$r.err &

#half-field 4v4 from checkpoint
#../build/release/experiments_robocup/sbbsoc -R -s $r -C 1 -T 60 -L 0 -t 60 -l 0 -A 4 -B 4 -f 17 -Y 1 1> sbb.$r.std 2> sbb.$r.err &

#half-field 4v4 replay 
#../build/release/cpp/experiments_robocup/exp_robocup -m -N -s $r -p 2 -t 0 -l 1 -i 1 -g 1000 -A 4 -B 4 -f 17 -Y 1 1> sbb.$r.std 2> sbb.$r.err &

#half-field 4v4 replay for pareto selection (keepaway)
#../build/release/experiments_robocup/sbbsoc -R -s $r -p 1 -t 125 -l 0 -i 1 -g 25 -A 4 -B 4 -f 17 -Y 1 1> sbb.$r.std 2> sbb.$r.err &

#half-field 4v4 replay for pareto selection (closeGoal)
#../build/release/experiments_robocup/sbbsoc -s $r -p 1 -t 60 -l 0 -i 1 -g 25 -A 4 -B 4 -f 17 -Y 1 1> sbb.$r.std 2> sbb.$r.err &

#half-field 4v4 from checkpoint for learned policy switching
#../build/release/experiments_robocup/sbbsoc -O 5 -X 1 -R -s $r -H -Q 50 -C 1 -T 125 -L 0 -t 125 -l 0 -A 4 -B 4 -f 17 -Y 1 1> sbb.$r.std 2> sbb.$r.err & 

#half-field 4v4 from checkpoint for continueation of learned policy switching run
#../build/release/experiments_robocup/sbbsoc -R -s $r -C -1 -T 125 -L 1 -t 125 -l 1 -A 4 -B 4 -f 17 -Y 1 1> sbb.$r.std 2> sbb.$r.err &

