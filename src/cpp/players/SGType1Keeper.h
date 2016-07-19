#ifndef SGTYPE1KEEPER
#define SGTYPE1KEEPER

#include "ShootgoalPlayer.h"
#include "SarsaAgent.h"
#include "SBBAgent.h"
#include "PolicyAgent.h"

class SGType1Keeper:public ShootgoalPlayer
{
  private:
  
  SarsaAgent *SA;
  SBBAgent *SBBA;
  
  static constexpr double discountFactor = 0;
  
  static constexpr double goalReward = 1.0;
  static constexpr double passReward = 0;
  static constexpr double dribbleReward = 0;
  static constexpr double ballTakenReward = -0.2;//-0.5;//-0.2;//-0.3;//-0.2;
  static constexpr double ballOutOfPlayReward = -0.1;//-0.5;//-0.1;//-0.3;//-0.1;
  static constexpr double goalieCatchReward = -0.1;//-0.5;//-0.1;//-0.3;//-0.1; 
 
  double currentReward;
  double multiplier;
  
  bool dumpMode;
  
  int m_transferType;
  char *m_transferWeightsFile;
  clock_t    start;
  
  PolicyAgent* m_transferAgent;
  
  char *m_dumpWeightsFile;
  char *m_dumpLogFile;
  char *m_stateLogFile;

  Time          m_timeStartEpisode;

public:
  
  	SGType1Keeper( SarsaAgent  *sa, SBBAgent *sbba, ActHandler *act, WorldModel *wm, 
			ServerSettings *ss, PlayerSettings *ps,
			char* strTeamName, int id, int iNumKeepers, int iNumTakers,
			bool isGoalie,
			double dVersion, int startEpisode, int transferType, PolicyAgent *transferAgent, 
			char *dumpWeightsFile, char *dumpLogFile, char *stateLogFile, int iReconnect, int iStopAfter, int iPolicy );


	SoccerCommand player(int & m_episodeCount);
	SoccerCommand keeperWithBall();

} ;

#endif
