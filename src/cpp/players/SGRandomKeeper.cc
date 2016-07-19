/*
Copyright (c) 2004 Gregory Kuhlmann, Peter Stone
University of Texas at Austin
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the University of Amsterdam nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "SGRandomKeeper.h"
#include "Parse.h"
#include "SayMsgEncoder.h"

extern LoggerDraw LogDraw;

SGRandomKeeper::SGRandomKeeper( RandomAgent  *ra, ActHandler *act, WorldModel *wm, 
				ServerSettings *ss, PlayerSettings *ps,
				char *strTeamName, int iNumKeepers, int iNumTakers,
				bool isGoalie,
				double dVersion, int startEpisode, char *stateLogFile, int iReconnect, int iStopAfter ):
		ShootgoalPlayer( ra, act, wm, ss, ps, strTeamName, iNumKeepers, iNumTakers, isGoalie, dVersion, startEpisode, iReconnect, iStopAfter )		
{
	RA = ra;
	WM->setTaskType(1);	
	WM->resetNewTrainerMessageHeard();  
}


SoccerCommand SGRandomKeeper::player(int & episodeCount)
{
  SoccerCommand soc;

  if(WM->isNewTrainerMessageHeard())
  {
     if ( WM->isNewEpisode() )
        episodeCount++;
  }

  // If we don't know where the ball is, search for it.
  if ( WM->getConfidence( OBJECT_BALL ) <
       PS->getBallConfThr() ) {
    ACT->putCommandInQueue( soc = searchBall() );
    ACT->putCommandInQueue( alignNeckWithBody() );
    LogDraw.logText( "state", VecPosition( 25, 25 ),
		     "lost ball",
		     1, COLOR_WHITE );
	return soc;
  }		     
	
  // If the ball is kickable,
  // call main action selection routine.
  if ( WM->isBallKickable() ) {
    Log.log( 100, "Ball is kickable for me." );
    return keeperWithBall();
  }

  // Get fastest to ball
  int iTmp;
  ObjectT fastest = WM->getFastestInSetTo( OBJECT_SET_TEAMMATES, 
					   OBJECT_BALL, &iTmp );
  
  // If fastest, intercept the ball.
  if ( fastest == WM->getAgentObjectType() ) {
    Log.log( 100, "I am fastest to ball; can get there in %d cycles", iTmp );
    LogDraw.logText( "state", VecPosition( 25, 25 ),
		     "fastest",
		     1, COLOR_WHITE );

    ObjectT lookObject = chooseLookObject( 0.98 );

    char buffer[128];
    LogDraw.logText( "lookObject", VecPosition( 25, -25 ), 
		     SoccerTypes::getObjectStr( buffer, lookObject ), 100, COLOR_WHITE );

    ACT->putCommandInQueue( soc = intercept( false ) );
    ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    return soc;
  }

  // Not fastest, get open
  Log.log( 100, "I am not fastest to ball" );
  LogDraw.logText( "state", VecPosition( 25, 25 ),
		   "support",
		   1, COLOR_WHITE );
  return keeperSupport( fastest );
}    

SoccerCommand SGRandomKeeper::keeperWithBall()
{
  double state[ MAX_STATE_VARS ];
  int action;
  
  WM->SGkeeperStateVars( state );
  
  action = RA->selectAction(state,-1);
    
  return interpretKeeperAction(action);
}

