#ifndef SGHANDCODEDKEEPER
#define SGHANDCODEDKEEPER

#include "ShootgoalPlayer.h"
#include "SGHandcodedAgent.h"


class SGHandcodedKeeper:public ShootgoalPlayer
{
  private:
  	SGHandcodedAgent *HA;

  public:
  
  	SGHandcodedKeeper( SGHandcodedAgent  *ha, ActHandler *act, WorldModel *wm, 
			ServerSettings *ss, PlayerSettings *ps,
			char* strTeamName, int id, int iNumKeepers, int iNumTakers,
			bool isGoalie,
			double dVersion, int startEpisode, char *stateLogFile, int iReconnect, int iStopAfter );


	SoccerCommand player(int & episodeCOunt);
	SoccerCommand keeperWithBall();

} ;

#endif

