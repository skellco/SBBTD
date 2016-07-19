#ifndef KEEPAWAYKEEPER
#define KEEPAWAYKEEPER

#include "KeepawayPlayer.h"
#include "SarsaAgent.h"
#include "SBBAgent.h"

class KeepawayType1Keeper:public KeepawayPlayer
{

 private:
  
	SarsaAgent *SA;
        SBBAgent *SBBA;
	int lastActionTime;
        int lastActionTimeWithoutBall;
        Time          m_timeStartEpisode;
 
 public:
  
  	KeepawayType1Keeper( SarsaAgent  *sa, SBBAgent *sbba, ActHandler *act, WorldModel *wm, 
			ServerSettings *ss, PlayerSettings *ps,
			char* strTeamName, int iNumKeepers, int iNumTakers,
			bool isGoalie,
			double dVersion, int startEpisode, int transferType, char *transferWeightsFile, 
			char *dumpWeightsFile, char *dumpLogFile, char *stateLogFile, int iReconnect, int iStopAfter, int iPolicy );


	SoccerCommand player(int & m_episodeCount);
	SoccerCommand keeperWithBall();

} ;

#endif
