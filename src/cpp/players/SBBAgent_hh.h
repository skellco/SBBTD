#ifndef LEARNING_AGENT_MASTER
#define LEARNING_AGENT_MASTER
#include "SMDPAgent.h"
#include "WorldModel.h"
#include "point.h"
#include "team.h"
#include "learner.h"
#include "SBBMod.h"
#include "behaviouralTypes.h"

//#include <boost/interprocess/shared_memory_object.h>
//#include <boost/interprocess/mapped_region.h>
//#include <boost/interprocess/sync/interprocess_semaphore.h>
//#include "semaphore_shared_data.h"

class SBBAgent_hh:public SMDPAgent
{
	SBBMod sbb;

	/* this m_ nonesense is left over from standard keepaway agent code. */
	bool m_learning, m_saving;
	char m_saveWeightsFile[128];
	int m_action;
	int m_previousAction;
	double m_lastState[MAX_STATE_VARS];
	int m_id;
	int m_stopAfter;
	int m_numKeepers;
	long m_t;
	long m_level;
	int m_phase;
	//for normalizing and discretizing state
	double maxDistToC;
	double maxDist;
	double maxAng;
	double minPassTime;
	double maxPassTime;

	//timing
	int timeTestSec0;
	int timeTestSec1;
	int timeGenSec0;
	int timeGenSec1;
	double sys1, usr1, sys2, usr2;

	long epochNum;

	//logging
	ostringstream oss;

	//SBB
	vector < double > behaviourSequence;
	double rewardSum;
	long m_step;

	//for reporting only
	vector < learner * > winner; /* Winner at each level. */
	vector < double > bid1; /* First and second highest bid at each level. */
	vector < double > bid2;

	void loadWeights ( char  *filename ); //unused
	void saveWeights ( char  *filename ); //unused
	void update      ( double state[], int action, double reward ); //unused
	int  selectAction( double state[] ); //unused

public:
	SBBAgent_hh( int   numFeatures,
			int   numActions,
			bool  learning,
			int phase,
			int t,
			int level,
			int numKeepers,
			int id,
			int seed,
			int stopAfter);

	int  startEpisode( double state[], double simTime );
	int  step( double reward, double state[], double simTime );
	void endEpisode( double reward, double state[], double simTime );
	void setParams(int iCutoffEpisodes, int iStopLearningEpisodes);
};

#endif
