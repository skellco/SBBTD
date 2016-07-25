#include <fcntl.h>
#include "SBBAgent.h"
#include "LoggerDraw.h"
#include "FuncApprox.h"
#include "CMAC.h"
#include <fstream>
#include <math.h>
#include <cstring>
#include <string.h>  // for memcpy
#include <stdlib.h>  // for rand
#include "sbbTD.h"
#include <iterator>
#include <time.h>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include "Logger.h"
#include <unistd.h>
#include <limits>
#include "ShootgoalPlayer.h"
#include "WorldModel.h"
#include <lo/lo.h>
#include <lo/lo_cpp.h>

//#define TIMING
extern LoggerDraw LogDraw;
using std::numeric_limits;

SBBAgent::SBBAgent( int numFeatures, int numActions, bool bLearn,
      FunctionApproximator *anFA, 
      char *loadWeightsFile, char *saveWeightsFile,
      int phase,
      int t,
      int level,
      int numKeepers,
      int id,
      int seed,
      int stopAfter,
      int taskType )
:PolicyAgent(numFeatures, numActions, id)			
{
   bLearning = bLearn;

   lastAction = -1;
   sbb.seed(seed);
   sbb.dim(numFeatures);
   sbb.numLevels(level+1);
   sbb.id(id);
   sleep(1);
   sbb.setParams();
   if (taskType == TASK_KEEPAWAY)
      sbb.numAtomicActions(numKeepers);
   else if (taskType == TASK_SHOOTGOAL)
      sbb.numAtomicActions(numKeepers + 1); 
   sbb.numStoredOutcomesPerHost(TRAIN_PHASE,2*numKeepers*sbb.episodesPerGeneration());
   sbb.numStoredOutcomesPerHost(VALIDATION_PHASE,numKeepers*sbb.validPhaseEpochs());
   sbb.numStoredOutcomesPerHost(TEST_PHASE,numKeepers*sbb.testPhaseEpochs());
   sbb.numStoredOutcomesPerHost(PLAY_PHASE,numKeepers*sbb.testPhaseEpochs());
   m_numKeepers = numKeepers;
   m_stopAfter = stopAfter;
   m_taskType = taskType;
   m_step = 0;
   epochNum = -1;
   m_id = sbb.id();
   m_t= t;
   m_level = level;
   m_phase = phase;
   updateReward = -77,0;
   minDconeAvg = 0.0;
   scoringWindowAvg = 0.0;
   distToGoalAvg = 0.0;
   numShoot = 0;
   numDribble = 0;
   numPass = 0;
   closeGoalLogged = false;

   for (int l = 0; l < sbb.numLevels(); l++) learnersRanked.push_back(set < learner*,LearnerBidLexicalCompare>());
   behaviourSequence.clear();

   //  sprintf( inputFilename, "checkpoints/cp.%d.%d.%d.%d.%d.rslt",level,t,1,sbb.seed(),m_phase);
   //ifs.open(inputFilename, ios::in);
   //if (!ifs) {
   //   oss.str("");
   //   oss << "Can't open checkpoint file:" << inputFilename;
   //   die(__FILE__, __FUNCTION__, __LINE__, oss.str().c_str());
   //   oss.str("");
   //}
   prepareInFile(ifs,"checkpoints","cp",level,t,1,sbb.seed(),m_phase);
   sbb.readCheckpoint(m_phase,ifs); //homo, always read checkpoint with id 1
   ifs.close();
   //normal sbb
   sbb.makeEvaluationVector(t, m_phase,true,m_stopAfter); //use tempNumOutcomes
   //pareto
   //sbb.makeEvaluationVector(t, m_phase,true,25); //use tempNumOutcomes

   //   if (sbb.evaluationVectorSize() != m_stopAfter && m_phase != PLAY_PHASE){
   //      cout << "SBBAgent_hh::error evaluation vector and m_stopAfter don't match phase " << m_phase << " " << sbb.evaluationVectorSize() << ":" << m_stopAfter << endl;
   //      exit(1);
   //   }

   sbb.getFirstTeam();

   timeGenSec0 = time(NULL);
}

/***********************************************************************************************************/
void SBBAgent::update(double state[], int action, double reward, double discountFactor, double episodeTime)
{
   updateReward = reward;
}

/***********************************************************************************************************/
void SBBAgent::endEpisode(double reward, double episodeTime)
{
   lo::Address policyAnimator("localhost", sbb.seed()+3);
   epochNum++;
   if (epochNum > 0){

      if(!bLearning)
         return;

      if (m_taskType == TASK_SHOOTGOAL){
         reward = updateReward;//the last update reward

         //only store the outcome if the reward is a valid enEpisode reward
         if (isEqual(reward,-0.1) || isEqual(reward,-0.2) || isEqual(reward,1.0)){
            //normal sbb
            sbb.setOutcome(behaviourSequence,behaviourSequence.size(),1.0 + reward,isEqual(reward,1.0)?reward:0.0,0.0,m_phase,m_t); //sarsa reward
            //pareto keepaway
            //sbb.setOutcome(behaviourSequence,behaviourSequence.size(),m_step>0?1/(distToGoalAvg/m_step):-1,m_step>0?minDconeAvg/m_step:-1,numPass,m_phase,m_t); 
            //pareto closeGoal
            //sbb.setOutcome(behaviourSequence,behaviourSequence.size(),m_step>0?scoringWindowAvg/m_step:-1,m_step>0?minDconeAvg/m_step:-1,1.0 + reward,m_phase,m_t);
         }
      }

      else if (m_taskType == TASK_KEEPAWAY)
         sbb.setOutcome(behaviourSequence,behaviourSequence.size(),episodeTime,episodeTime,episodeTime,m_phase,m_t);

      if (m_phase == PLAY_PHASE){
         cout << "endEpisode m_id " << m_id << " epochNum " << epochNum << " m_stopAfter " << m_stopAfter << " host " << sbb.activeTeamId() << " reward " << 1.0 + reward << " episodeTime " << episodeTime;
         cout << " distToGoalPart " << (m_step>0?1/(distToGoalAvg/m_step):-1)  << " DconePart " << (m_step>0?minDconeAvg/m_step:-1);
         cout << " scoringWindowPart " << (m_step>0?scoringWindowAvg/m_step:-1) << " numShoot " << numShoot << " numDribble " << numDribble;
         cout << " numPass " << numPass << endl;
         policyAnimator.send("score","i",(1.0 + reward > 1 ? 1 : 0));
      }

      behaviourSequence.clear();
      if (m_phase != TEST_PHASE && sbb.mSize() > 1)
         sbb.getNextTeam();

      if (epochNum >= m_stopAfter)
         sbb.writeEval(m_t,m_level,m_phase);

      m_step = 0;
      minDconeAvg = 0.0;
      scoringWindowAvg = 0.0;
      distToGoalAvg = 0.0;
      numShoot = 0;
      numDribble = 0;
      numPass = 0;
      closeGoalLogged = false;
   }
}

/***********************************************************************************************************/
void SBBAgent::reset()
{
   lastAction = -1;
}

/***********************************************************************************************************/
int SBBAgent::selectAction(double state[], double episodeTime)
{
#ifndef TIMING
   if (m_taskType == TASK_SHOOTGOAL){
      minDconeAvg += state[9];
      scoringWindowAvg += state[15];
      distToGoalAvg += state[16];
   }
#endif
   /* SBB policy */
   //lo::Address policyAnimator("localhost", sbb.seed()+3);
   //long winningSymbiont_ID_1;
   //long winningSymbiont_ID_0;
   long decisionInstructions = 0;
   vector <long> policyTreeTraceIds;
   vector <double> stateVec(state, state + sbb.dim());
   for (int l = 0; l < sbb.numLevels(); l++) learnersRanked[l].clear();
   m_action = sbb.getAction(stateVec, true, learnersRanked, decisionInstructions,policyTreeTraceIds); //warning: updateActive set to true

   ////tracing root->leaf path through policy tree for animation
   //winningSymbiont_ID_1 = (*(learnersRanked[1].begin()))->id();
   //winningSymbiont_ID_0 = (*(learnersRanked[0].begin()))->id();

#ifndef TIMING
   if (m_phase == PLAY_PHASE){
      cout << "bTrace " << vecToStr(stateVec) << " act " << m_action << endl;
      cout << "decisionInstructions " << decisionInstructions << endl;
      cout << "policyTrace episodeTime " << episodeTime << " playerid  " << sbb.id() << " " << policyTreeTraceIds[0] << " " << m_action << endl;
   }
#endif

   //cout.precision(numeric_limits< double >::digits10+1);
   ////cout << "featureToAct " << vecToStr(stateVec); //uncomment to use varify_policy      
   //cout << "bidRank " << learnersRanked[0].size() << " " << learnersRanked[1].size() << " act " << m_action << " lev0";
   ////cout << "bidRank  act " << m_action << " size " << learnersRanked[0].size() << " act " << m_action;
   ////uncomment to use varify_policy_number from main_sbb_hh.cc
   //for(set<learner*, LearnerBidLexicalCompare > :: iterator it = learnersRanked[0].begin(); it != learnersRanked[0].end();++it)
   //{
   //	cout << " [" << (*it)->key() << ",";
   //        cout << (*it)->esize() << ",";
   //        cout << (*it)->refs() << ",";
   //        cout << (*it)->numFeatures() << ",";
   //        cout << (*it)->gtime() << ",";
   //        cout << (*it)->ancestralGtime() << ",";
   //        cout << (*it)->id() << ",";
   //        cout << (*it)->lastCompareFactor() << "]";
   //}
   //cout << " lev1";
   //for(set<learner*, LearnerBidLexicalCompare> :: iterator it = learnersRanked[1].begin(); it != learnersRanked[1].end();++it)
   //{
   //cout << " [" << (*it)->key() << ","; 
   //cout << (*it)->esize() << ","; 
   //cout << (*it)->refs() << ",";
   //cout << (*it)->numFeatures() << ",";
   //cout << (*it)->gtime() << ","; 
   //cout << (*it)->ancestralGtime() << ","; 
   //cout << (*it)->id() << ","; 
   //cout << (*it)->lastCompareFactor() << "]";
   //}
   //cout << endl; //uncomment to use varify_policy

   //cout << "lev0Active act " << m_action;
   //cout << " [" << (*(learnersRanked[0].begin()))->key() << ","; 
   //cout << (*(learnersRanked[0].begin()))->esize() << ","; 
   //cout << (*(learnersRanked[0].begin()))->refs() << ",";
   //cout << (*(learnersRanked[0].begin()))->numFeatures() << ",";
   //cout << (*(learnersRanked[0].begin()))->gtime() << ","; 
   //cout << (*(learnersRanked[0].begin()))->ancestralGtime() << ","; 
   //cout << (*(learnersRanked[0].begin()))->id() << ","; 
   //cout << (*(learnersRanked[0].begin()))->lastCompareFactor() << "]";
   //cout << endl;

   //m_action = (int) (drand48() * sbb.numActions(0)); //random action
#ifndef TIMING
   behaviourSequence.push_back((double)((m_action*-1)-1)); //actions are represented as negatives
   if (m_taskType == TASK_SHOOTGOAL)
      halfFieldDiscretizeState(stateVec, sbb.stateDiscretizationSteps());
   else if (m_taskType == TASK_KEEPAWAY)
      keepawayDiscretizeState(stateVec, sbb.stateDiscretizationSteps());
   behaviourSequence.insert(behaviourSequence.end(),stateVec.begin(),stateVec.end());

   if (m_taskType == TASK_SHOOTGOAL){
      if (m_action > 1) 
         numPass++;
      else if (m_action == 1)
         numDribble++;
      else if (m_action == 0)
         numShoot++;
   }
#endif
   lastAction = m_action;
   m_step++;
   //if (m_phase == PLAY_PHASE){
   //   policyAnimator.send("act","ii",winningSymbiont_ID_1,winningSymbiont_ID_0);
   //   cout << "winningSymbiont_ID_1 " << winningSymbiont_ID_1 << endl;
   //   cout << "winningSymbiont_ID_0 " << winningSymbiont_ID_0 << endl;
   //}
   return m_action;

   ///*****************************************************************
   // * Hand Coded Policy                         
   // *****************************************************************/
   //int numK = 4;
   //int numT = 5;

   //int j = 0;

   //double disK12 = state[j++];
   //double disK13 = state[j++];
   //double disK14 = state[j++];

   //double K2MinDisT = state[j++];
   //double K3MinDisT = state[j++];
   //double K4MinDisT = state[j++];

   //double K2MinAngCloseT = state[j++];
   //double K3MinAngCloseT = state[j++];
   //double K4MinAngCloseT = state[j++];

   //double K1MinDisTInCone = state[j++];

   //double K1MinDisT = state[j++];

   //double K1DisGoal = state[j++];
   //double K2DisGoal = state[j++];
   //double K3DisGoal = state[j++];
   //double K4DisGoal = state[j++];

   //double K1MaxAngGoal = state[j++];

   //double K1DisGoalie = state[j++];

   //double pv2 = passValue(disK12, K2MinDisT, K2MinAngCloseT, K2DisGoal);
   //double pv3 = passValue(disK13, K3MinDisT, K3MinAngCloseT, K3DisGoal);
   //double pv4 = passValue(disK14, K4MinDisT, K4MinAngCloseT, K4DisGoal);

   //int action;

   //if((K1DisGoal < 15.0) && (K1MaxAngGoal > 20.0))
   //{
   //   action = ACTION_SG_SHOOT_GOAL;
   //}
   //else if((K1DisGoal < 25.0) && (K1MaxAngGoal > 40.0))
   //{
   //   action = ACTION_SG_SHOOT_GOAL;
   //}
   //else if((K1MinDisT > 6.0) && (K1MinDisTInCone > 10.0))
   //{
   //   action = ACTION_SG_DRIBBLE_NORMAL;
   //   //action = ACTION_SG_DRIBBLE_TO_GOAL;
   //}
   //else if((pv2 > pv3) && (pv2 > pv4))
   //{
   //   action = ACTION_SG_PASS_TO_K_2;
   //}
   //else if(pv3 > pv4)
   //{
   //   action = ACTION_SG_PASS_TO_K_3;
   //}
   //else
   //{
   //   action = ACTION_SG_PASS_TO_K_4;
   //}

   //return action;
}
/***********************************************************************************************************/
/* This method for hand-coded policy only. */
double SBBAgent::passValue(double dis, double minDisT, double minAngT, double disGoal)
{

   double value;
   bool canPass;

   if(dis > 30.0)
      canPass = false;
   else if(((minDisT < 6.0) || (minAngT < 20.0)) && (disGoal > 20.0))
      canPass = false;
   else if(((minDisT < 4.0) || (minAngT < 15.0)) && (disGoal <= 20.0))
      canPass = false;
   else
      canPass = true;

   if(canPass)
   {
      value = 1000.0 - disGoal;
      return value;
   }

   value = minAngT;
   return value;
}

/***********************************************************************************************************/
int SBBAgent::bestAction(double state[])
{
   return selectAction(state,-1);
}
