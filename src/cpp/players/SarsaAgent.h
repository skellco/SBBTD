#ifndef SARSA_AGENT
#define SARSA_AGENT

#include "PolicyAgent.h"
#include "FuncApprox.h"

#define MAX_STATE_VARS         64
#define MAX_ACTIONS            10

class SarsaAgent:public PolicyAgent
{
  
 public:
  char weightsFile[256];
  bool bLearning;
  bool bSaveWeights;
  
  int epochNum;
  double lastState[MAX_STATE_VARS];
  int lastAction;
  double lastReward;
  
  double alpha;
  double gamma;
  double lambda;
  double epsilon;

  double rewardSum;
  int goal;

  FunctionApproximator * FA;
  
  FunctionApproximator * LearnedFA;

  double Q[ MAX_ACTIONS ];

  // Load / Save weights from/to disk
  bool loadWeights( char *filename );
  bool saveWeights( char *filename );

  // Value function methods for CMACs
  int  argmaxQ(double state[]);

 public:
  SarsaAgent                        ( int    numFeatures,
				      int    numActions,
				      bool   bLearn,
				      // the function approximator should be created 
				      // with ranges, resolutions, numbers of features and actions
				      FunctionApproximator *anFA, 
				      char   *loadWeightsFile = "",
				      char   *saveWeightsFile = "",
                                      int id=-1);

  // SMDP Sarsa implementation
  int  selectAction(double state[], double episodeTime);
  void update(double state[], int action, double reward, double discountFactor, double simTime);
  void endEpisode(double, double simTime);
  void reset();

  int bestAction(double state[]);

  double computeQ(double state[], int action);

  double passValue(double dis, double minDisT, double minAngT, double disGoal); // for hand-coded policy only
} ;

#endif
