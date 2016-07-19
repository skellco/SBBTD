#ifndef SGTAKER
#define SGTAKER

#include "ShootgoalPlayer.h"

class SGTaker:public ShootgoalPlayer
{
 public:
 int m_id; 
  	SGTaker( SarsaAgent  *sa, ActHandler *act, WorldModel *wm, 
			ServerSettings *ss, PlayerSettings *ps,
			char* strTeamName, int id, int iNumKeepers, int iNumTakers,
			bool isGoalie,
			double dVersion, int startEpisode, char *stateLogFile, int iReconnect, int iStopAfter );


	SoccerCommand player(int & episodeCount);
} ;

#endif
