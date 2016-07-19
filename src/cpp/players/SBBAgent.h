#ifndef SBB_AGENT
#define SBB_AGENT

#include "PolicyAgent.h"
#include "FuncApprox.h"
#include "SMDPAgent.h"
#include "WorldModel.h"
#include "sbbPoint.h"
#include "sbbTeam.h"
#include "sbbLearner.h"
#include "sbbTD.h"
#include "behaviouralTypes.h"
#include "soccer.h"

#define MAX_STATE_VARS         64
#define MAX_ACTIONS            10

class SBBAgent:public PolicyAgent
{

	private:
		double passValue(double dis, double minDisT, double minAngT, double disGoal);

		sbbTD sbb;
		/* this m_ nonesense is left over from standard keepaway agent code. */
		bool m_learning, m_saving;
		char m_saveWeightsFile[128];
		int m_action;
		double m_lastState[MAX_STATE_VARS];
		int m_id;
		int m_stopAfter;
		int m_numKeepers;
		long m_t;
		long m_level;
		int m_phase;
		double minPassTime;
		double maxPassTime;

		//timing
		int timeTestSec0;
		int timeTestSec1;
		int timeGenSec0;
		int timeGenSec1;
		double sys1, usr1, sys2, usr2;

		//logging
		ostringstream oss;

		//SBB
		vector < double > behaviourSequence;
                vector < set <learner*, LearnerBidLexicalCompare> > learnersRanked;
		double updateReward;
                double minDconeAvg;
                double scoringWindowAvg;
                double distToGoalAvg;
                int numShoot;
                int numDribble;
                int numPass;
		double goal;
                double ballTaken;
		long m_step;
                long m_simTime;
                bool closeGoalLogged;
                ifstream ifs;
                char inputFilename[80];

		//for reporting only
		vector < learner * > winner; /* Winner at each level. */
		vector < double > bid1; /* First and second highest bid at each level. */
		vector < double > bid2;
	public:

		bool bLearning;

		int epochNum;
		double lastState[MAX_STATE_VARS];
		int lastAction;
		double lastReward;
		int m_taskType;

	public:
		SBBAgent                        ( int    numFeatures,
				int    numActions,
				bool   bLearn,
				// the function approximator should be created 
				// with ranges, resolutions, numbers of features and actions
				FunctionApproximator *anFA, 
				char   *loadWeightsFile = "",
				char   *saveWeightsFile = "",
				int phase=0,
				int t=0,
				int level=0,
				int numKeepers=0,
				int id=0,
				int seed=0,
				int stopAfter=0,
				int taskType=TASK_KEEPAWAY);

		// SMDP Sarsa implementation
		int  selectAction(double state[], double episodeTime);
		void update(double state[], int action, double reward, double discountFactor, double episodeTime);
		void endEpisode(double reward,double episodeTime);
		void reset();

		int bestAction(double state[]);

} ;

#endif
