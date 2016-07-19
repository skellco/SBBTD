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

#include "KeepawayType1Keeper.h"
#include "Parse.h"
#include "SayMsgEncoder.h"

extern LoggerDraw LogDraw;

KeepawayType1Keeper::KeepawayType1Keeper( SarsaAgent  *sa, SBBAgent *sbba, ActHandler *act, WorldModel *wm, 
      ServerSettings *ss, PlayerSettings *ps,
      char *strTeamName, int iNumKeepers, int iNumTakers,
      bool isGoalie,
      double dVersion, int startEpisode, int transferType, char *transferWeightsFile, 
      char *dumpWeightsFile, char *dumpLogFile, char *stateLogFile, int iReconnect, int iStopAfter, int iPolicy ):
   KeepawayPlayer( sa, act, wm, ss, ps, strTeamName, iNumKeepers, iNumTakers, isGoalie, dVersion, startEpisode, iReconnect, iStopAfter, iPolicy )		
{
   SA = sa;
   SBBA = sbba;
   WM->setTaskType(TASK_KEEPAWAY);
   WM->resetNewTrainerMessageHeard();
   lastActionTime = -1;
   lastActionTimeWithoutBall = -1;
   lastAction = -1;
}


SoccerCommand KeepawayType1Keeper::player(int & m_episodeCount)
{
   SoccerCommand soc;

   if ( WM->isNewEpisode() )
   {
      m_episodeCount++;
      double reward = WM->getCurrentCycle() - lastActionTime;
      //if(lastAction != -1)
      //{
      //        if (m_policy == 1)
      //           SA->update(lastState, lastAction, reward, 1.0, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());//multiplier a dummy.
      //       
      //}
      //if (m_policy == 0)
      //   SBBA->update(lastState, lastAction, reward, 1.0, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());//multiplier a dummy.

      if (m_policy == 1)
	 SA->endEpisode(reward, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
      else
	 SBBA->endEpisode(reward, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());	

      WM->setNewEpisode( false );
      WM->setLastAction( UnknownIntValue );
      lastAction = -1;
      m_timeStartEpisode = WM->getCurrentTime();
   }
   else {
      if (m_policy == 1){
	 double reward = WM->getCurrentCycle() - lastActionTimeWithoutBall;
	 SA->update(lastState, lastAction, reward, 1.0, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
      }

   }

   // If we don't know where the ball is, search for it.
   if ( WM->getConfidence( OBJECT_BALL ) <
	 PS->getBallConfThr() ) {
      //cout << "we don't know where the ball is, search for it." << endl;
      ACT->putCommandInQueue( soc = searchBall() );
      ACT->putCommandInQueue( alignNeckWithBody() );
      LogDraw.logText( "state", VecPosition( 25, 25 ),
	    "lost ball",
	    1, COLOR_WHITE );
      lastActionTimeWithoutBall = WM->getCurrentCycle();
      return soc;
   }

   // If the ball is kickable,
   // call main action selection routine.
   if ( WM->isBallKickable() ) {
      //cout << "the ball is kickable" << endl;
      Log.log( 100, "Ball is kickable for me." );
      return keeperWithBall();
   }

   // Get fastest to ball
   int iTmp;
   ObjectT fastest = WM->getFastestInSetTo( OBJECT_SET_TEAMMATES, 
	 OBJECT_BALL, &iTmp );

   // If fastest, intercept the ball.
   if ( fastest == WM->getAgentObjectType() ) {
      //cout << "I am fastest to ball" << endl;
      Log.log( 100, "I am fastest to ball; can get there in %d cycles", iTmp );
      LogDraw.logText( "state", VecPosition( 25, 25 ),
	    "fastest",
	    1, COLOR_WHITE );

      ObjectT lookObject = chooseLookObject( 0.98 );

      char buffer[128];
      LogDraw.logText( "lookObject", VecPosition( 25, -25 ), 
	    SoccerTypes::getObjectStr( buffer, lookObject ), 100, COLOR_WHITE );

      ACT->putCommandInQueue( soc = intercept( false ) );
      //ACT->putCommandInQueue( turnNeckToObject( lookObject, soc ) );
      //ACT->putCommandInQueue( turnNeckToPoint( WM->getKeepawayRect().getPosCenter(), soc ) );
      ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
      return soc;
   }

   // Not fastest, get open
   //cout << "Not fastest, get open" << endl;
   Log.log( 100, "I am not fastest to ball" );
   LogDraw.logText( "state", VecPosition( 25, 25 ),
	 "support",
	 1, COLOR_WHITE );

   return keeperSupport( fastest );
}    

SoccerCommand KeepawayType1Keeper::keeperWithBall()
{
   double state[ MAX_STATE_VARS ];
   int action;

   if(lastAction != -1)
   {
      double reward = WM->getCurrentCycle() - lastActionTime;
      if (m_policy == 1)
	 SA->update(lastState, lastAction, reward, 1.0, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
      else
	 SBBA->update(lastState, lastAction, reward, 1.0, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
   }

   if ( WM->KWYkeeperStateVars( state ) > 0 ) 
   { // if we can calculate state vars
      if( WM->getTimeLastAction() == WM->getCurrentCycle() - 1 && WM->getLastAction() > 0 ) 
      {   // if we were in the middle of a pass last cycle
	 action = WM->getLastAction();         // then we follow through with it
	 //cout << "we were in the middle of a pass last cycle" << endl;
      }
      else
      {
	 //cout << "selecting action" << endl;
	 if (m_policy == 1)
	    action = SA->selectAction(state, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
	 else
	    action = SBBA->selectAction(state, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
      }

      WM->setLastAction( action );////Originally here%%%
      WM->copyState(state, lastState);
      lastAction = action;
      lastActionTime = WM->getCurrentCycle();
   }
   else 
   { // if we don't have enough info to calculate state vars
      //cout << "we don't have enough info to calculate state vars" << endl;
      action = ACTION_KWY_DRIBBLE_NORMAL;  // Default 
      LogDraw.logText( "state", VecPosition( 35, 25 ),
	    "clueless",
	    1, COLOR_RED );
   }

   return interpretKeeperAction(action);
}

