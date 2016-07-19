#ifndef RANDOM_AGENT
#define RANDOM_AGENT

#include "PolicyAgent.h"

class RandomAgent:public PolicyAgent
{
  public:
  	RandomAgent( int numFeatures, int numActions, int id);
        int selectAction(double state[], double episodeTime);
        int bestAction(double state[]);
};

#endif


