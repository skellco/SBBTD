#ifndef SGRANDOMKEEPER
#define SGRANDOMKEEPER

#include "ShootgoalPlayer.h"
#include "RandomAgent.h"


class SGRandomKeeper:public ShootgoalPlayer
{
  private:
  	RandomAgent *RA;

  public:
  
  	SGRandomKeeper( RandomAgent  *ra, ActHandler *act, WorldModel *wm, 
			ServerSettings *ss, PlayerSettings *ps,
			char* strTeamName, int iNumKeepers, int iNumTakers,
			bool isGoalie,
			double dVersion, int startEpisode, char *stateLogFile, int iReconnect, int iStopAfter );

        SoccerCommand player(int & episodeCount);
	SoccerCommand keeperWithBall();

} ;

#endif

