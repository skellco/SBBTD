#ifndef SGHANDCODED_AGENT
#define SGHANDCODED_AGENT

#include "PolicyAgent.h"

class SGHandcodedAgent:public PolicyAgent
{
  private:
	double passValue(double dis, double minDisT, double minAngT, double disGoal);
  
  public:
  	SGHandcodedAgent( int numFeatures, int numActions, int id);
        int selectAction(double state[], double episodeTime);
        int bestAction(double state[]);

  SoccerCommand player(int & episodeCount);
};

#endif

