#ifndef KEEPAWAYTAKER
#define KEEPAWAYTAKER

#include "KeepawayPlayer.h"

class KeepawayType1Taker:public KeepawayPlayer
{

 public:
  
  	KeepawayType1Taker( SarsaAgent  *sa, ActHandler *act, WorldModel *wm, 
			ServerSettings *ss, PlayerSettings *ps,
			char* strTeamName, int iNumKeepers, int iNumTakers,
			bool isGoalie,
			double dVersion, int startEpisode, char *stateLogFile, int iReconnect, int iStopAfter );


	SoccerCommand player(int & episodeCount);
} ;

#endif

