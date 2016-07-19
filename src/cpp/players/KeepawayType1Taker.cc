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

#include "KeepawayType1Taker.h"
#include "Parse.h"
#include "SayMsgEncoder.h"

extern LoggerDraw LogDraw;

KeepawayType1Taker::KeepawayType1Taker( SarsaAgent  *sa, ActHandler *act, WorldModel *wm, 
				ServerSettings *ss, PlayerSettings *ps,
				char* strTeamName, int iNumKeepers, int iNumTakers,
				bool isGoalie,
				double dVersion, int startEpisode, char *stateLogFile, int iReconnect, int iStopAfter ):
		KeepawayPlayer( sa, act, wm, ss, ps, strTeamName, iNumKeepers, iNumTakers, isGoalie, dVersion, startEpisode, iReconnect, iStopAfter )		
{
}


SoccerCommand KeepawayType1Taker::player(int & episodeCount)
{
  if ( WM->isNewEpisode() )
  {
        episodeCount++;
        //cout << "Taker episodeCount " << episodeCount << endl;

        WM->setNewEpisode( false );
        WM->setLastAction( UnknownIntValue );

        m_episodeNumber++;
  }

  SoccerCommand soc;

  LogDraw.logCircle( "ball pos", WM->getBallPos(),
		     1.1, 11, false, COLOR_RED, WM->getConfidence( OBJECT_BALL ) );

  // If we don't know where the ball is, search for it.
  if ( WM->getConfidence( OBJECT_BALL ) <
       PS->getBallConfThr() ) {
    ACT->putCommandInQueue( soc = searchBall() );
    ACT->putCommandInQueue( alignNeckWithBody() );
    return soc;
  }
  
  int numT = WM->getNumTakers();
  ObjectT T[numT];

  for(int y = 0; y < numT; y++)
  	T[y] = SoccerTypes::getTeammateObjectFromIndex(y);
			
  WM->sortClosestTo( T, numT, OBJECT_BALL );
	
  int numK = WM->getNumKeepers();
  ObjectT K[numK];
	
  for(int y = 0; y < numK; y++)
 	K[y] = SoccerTypes::getOpponentObjectFromIndex(y);
			
  WM->sortClosestTo( K, numK, OBJECT_BALL );
  
  VecPosition ballPos = WM->getGlobalPosition(OBJECT_BALL);
  VecPosition myPos = WM->getGlobalPosition(WM->getAgentObjectType());

  // Maintain possession if you have the ball.
  if (WM->isBallKickable() && WM->getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL) == WM->getAgentObjectType()) 
  {
    ACT->putCommandInQueue( soc = holdBall() );
    return soc;
  }  

	
	ObjectT keeperWithBall = K[0];
	
	
	if((WM->getAgentObjectType() == T[0]) || (WM->getAgentObjectType() == T[1]))
	{
		soc = intercept(false);
			
		ACT->putCommandInQueue(soc);
		return soc;
	}	

	//soc = mark(SoccerTypes::getOpponentObjectFromIndex(WM->getPlayerNumber() - 1), 4.0, MARK_BALL);
	soc = markMostOpenOpponent(K[0]);
	ACT->putCommandInQueue(soc);

	return soc;
	
	
	

  // If teammate has it, don't mess with it
  double dDist;
  ObjectT closest = WM->getClosestInSetTo( OBJECT_SET_PLAYERS, 
					   OBJECT_BALL, &dDist );
  if ( SoccerTypes::isTeammate( closest ) &&
       closest != WM->getAgentObjectType() &&
       dDist < SS->getMaximalKickDist() ) {
    ACT->putCommandInQueue( soc = turnBodyToObject( OBJECT_BALL ) );
    ACT->putCommandInQueue( alignNeckWithBody() );
    return soc;
  }
}
