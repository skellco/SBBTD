#!/bin/bash
cat $1 | grep "seed:" > $3
echo "learnerPop:" >> $3
cat $1 | grep "learner:" >> $3
cat $2 | grep "learner:" >> $3
echo "endLearnerPop" >> $3
echo "teamPop:" >> $3
cat $1 | grep "team:" >> $3
cat $2 | grep "team:" >> $3
echo "endTeamPop" >> $3
