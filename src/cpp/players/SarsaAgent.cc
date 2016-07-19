#include <fcntl.h>
#include "SarsaAgent.h"
#include "LoggerDraw.h"
#include "FuncApprox.h"
#include "CMAC.h"
#include <fstream>
#include <math.h>
#include <cstring>
#include <sys/stat.h>
#include "ShootgoalPlayer.h"

// If all is well, there should be no mention of anything keepaway- or soccer-
// related in this file. 

extern LoggerDraw LogDraw;

// return a random integer in [0, maxRange)
inline int range_rand( int maxRange )
{
   return (int) ( drand48() * maxRange );
}

inline int range_rand( int minRange, int maxRange )
{
   return range_rand( maxRange - minRange ) + minRange;
}

SarsaAgent::SarsaAgent( int numFeatures, int numActions, bool bLearn,
      FunctionApproximator *anFA, 
      char *loadWeightsFile, char *saveWeightsFile, int id)
:PolicyAgent(numFeatures, numActions, id)			
{

   bLearning = bLearn;

   if ( bLearning && strlen( saveWeightsFile ) > 0 ) {
      strcpy( weightsFile, saveWeightsFile );
      bSaveWeights = true;
   }
   else {
      bSaveWeights = false;
   }

   alpha = 0.05;
   gamma = 1.0;
   lambda = 0;
   epsilon = 0.01;

   epochNum = 0;
   lastAction = -1;

   rewardSum = 0;
   goal = 0;

   FA = anFA;

   srand( time( NULL ) );

   if ( strlen( loadWeightsFile ) > 0 )
      loadWeights( loadWeightsFile );

}

void SarsaAgent::update(double state[], int action, double reward, double discountFactor, double simTime)
{
   if (reward > 0)
      goal = 1;

   rewardSum+=reward;

   if(!bLearning)
      return;

   if(lastAction == -1)
   {
      for(int i = 0; i < getNumFeatures(); i++)
	 lastState[i] = state[i];
      lastAction = action;
      lastReward = reward;	
   }
   else
   {
      FA->setState(lastState);
      FA->updateTraces(lastAction);
      double oldQ = FA->computeQ(lastAction);
      double delta = lastReward - oldQ;

      FA->setState(state);

      //SHIVA AFTER CHANGE FOR Q-LEARNING
      //double newQ = FA->computeQ(action);

      double newQ = FA->computeQ(argmaxQ(state));

      delta += discountFactor * newQ;

      FA->updateWeights(delta, alpha);
      FA->decayTraces(0);//Assume gamma, lambda are 0.

      for(int i = 0; i < getNumFeatures(); i++)
	 lastState[i] = state[i];
      lastAction = action;
      lastReward = reward;	
   }
}

void SarsaAgent::endEpisode(double reward, double simTime)
{	
   epochNum++;

   cout << "SarsaAgent epochNum " << epochNum << " goal " << goal << " rewardSum " << rewardSum << " simTime " << simTime << endl;
   rewardSum = 0;
   goal = 0;
   if(!bLearning)
      return;

   if(lastAction == -1)
   {
      return;//This will not happen usually during keepaway, but is a safety.
   }
   else
   {
      FA->setState(lastState);
      FA->updateTraces(lastAction);
      double oldQ = FA->computeQ(lastAction);
      double delta = lastReward - oldQ;

      FA->updateWeights(delta, alpha);
      FA->decayTraces(0);//Assume lambda is 0.
   }

   if ( bLearning && bSaveWeights && epochNum % 10000 == 0 )  //if ( bLearning && bSaveWeights && rand() % 200 == 0 )
   {
      cout << "SarsaAgent Saving " << epochNum << endl;
      char outputFilename[80];
      //mkdir("sarsa_weights", S_IRWXU|S_IRGRP|S_IXGRP);
      sprintf(outputFilename, "sarsa_weights/sarsa.%d.%s.%d.rslt",m_id,weightsFile,epochNum );
      saveWeights( outputFilename );

   }

   lastAction = -1;
}

void SarsaAgent::reset()
{
   lastAction = -1;
}

int SarsaAgent::selectAction(double state[], double episodeTime)
{
   /* Sarsa Policy */

   int action;

   // Epsilon-greedy
   if ( bLearning && drand48() < epsilon ) {     /* explore */
   	action = range_rand( getNumActions() );
   }
   else{
   	action = argmaxQ(state);
   }

   /*
      fstream file;
      file.open("./gamma.txt", ios::out | ios::app);
      file<<endl<<"Inside SARSA Agent, action: " << action << endl;
      file << "numActions: " << getNumActions();
      file.close();	
    */
   return action;

   ///* Hand coded policy */

   //cout << "Hand coded here!";
   //for (int i = 0; i < 17; i++)
   //   cout << " " << state[i];
   //cout <<  endl;
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

bool SarsaAgent::loadWeights( char *filename )
{
   char inputFilename[80];
   sprintf(inputFilename,filename);
   cout << "Loading weights from " << inputFilename << endl;

   int file = open( inputFilename, O_RDONLY );
   FA->read( file );
   close( file );
   cout << "...done" << endl;
   return true;
}

bool SarsaAgent::saveWeights( char *filename )
{
   int file = open( filename, O_CREAT | O_WRONLY, 0664 );
   FA->write( file );
   close( file );
   return true;
}

// Returns index (action) of largest entry in Q array, breaking ties randomly 
int SarsaAgent::argmaxQ(double state[])
{

   FA->setState(state);

   for(int i = 0; i < getNumActions(); i++)
      Q[i] = FA->computeQ(i);

   int bestAction = 0;
   double bestValue = Q[ bestAction ];
   int numTies = 0;
   for ( int a = bestAction + 1; a < getNumActions(); a++ ) {
      double value = Q[ a ];
      if ( value > bestValue ) {
	 bestValue = value;
	 bestAction = a;
      }
      else if ( value == bestValue ) {
	 numTies++;
	 if ( range_rand( numTies + 1 ) == 0 ) {
	    bestValue = value;
	    bestAction = a;
	 }
      }
   }

   return bestAction;
}

int SarsaAgent::bestAction(double state[])
{
   return argmaxQ(state);
}

double SarsaAgent::computeQ(double state[], int action)//Be careful--this resets FA->state
{
   FA->setState(state);
   double QValue = FA->computeQ(action);

   return QValue;
}

/* This method for hand-coded policy only. */
double SarsaAgent::passValue(double dis, double minDisT, double minAngT, double disGoal)
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

