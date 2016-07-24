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

#include "SGType1Keeper.h"
#include "Parse.h"
#include "SayMsgEncoder.h"

#include <iomanip>
#include <cstring>
#include <stdlib.h>
#include <ctime>
extern LoggerDraw LogDraw;

SGType1Keeper::SGType1Keeper( SarsaAgent  *sa, SBBAgent *sbba, ActHandler *act, WorldModel *wm, 
      ServerSettings *ss, PlayerSettings *ps,
      char *strTeamName, int id, int iNumKeepers, int iNumTakers,
      bool isGoalie,
      double dVersion, int startEpisode, int transferType, PolicyAgent *transferAgent, 
      char *dumpWeightsFile, char *dumpLogFile, char *stateLogFile, int iReconnect, int iStopAfter, int iPolicy ):
   ShootgoalPlayer( sa, act, wm, ss, ps, strTeamName, id, iNumKeepers, iNumTakers, isGoalie, dVersion, startEpisode, iReconnect, iStopAfter, iPolicy )		
{
   SA = sa;
   SBBA = sbba;
   WM->setTaskType(TASK_SHOOTGOALVOICED);	
   WM->resetNewTrainerMessageHeard();  
   WM->setNewEpisode( false );

   if(strlen(dumpWeightsFile) > 0)
   {
      dumpMode = true;
   }
   else
   {
      dumpMode = false;
   }	

   m_transferType = transferType;
   m_transferAgent = transferAgent;

   m_dumpWeightsFile = dumpWeightsFile;
   m_dumpLogFile = dumpLogFile; 
   m_stateLogFile = stateLogFile;
   m_timeStartEpisode = -5;
}

SoccerCommand SGType1Keeper::player(int & m_episodeCount)
{
   SoccerCommand soc;
   //cout << "PLAY_MODE " << WM->getPlayMode() << endl;
   //if (WM->getPlayMode() == 29){//game is frozen
   //   //STAY
   //   VecPosition destination;
   //   VecPosition myPosI = WM->getGlobalPosition(WM->getAgentObjectType());
   //   myPosI = WM->getGlobalPosition(WM->getAgentObjectType());
   //   destination = VecPosition(myPosI.getX(),myPosI.getY());
   //   soc = moveToPos(destination, 30, 5, false, 5);
   //   ACT->putCommandInQueue(soc);
   //   return soc;
   //}

   double reward = 7;
   if(WM->isNewTrainerMessageHeard())
   {
      fstream file;


      double state[100];
      int action;
      int gameCondition;
      //double reward;

      WM->getReceivedState(state);
      action = WM->getReceivedAction();
      gameCondition = WM->getGameCondition();

      bool gameConditionValid = true;

      switch(gameCondition)
      {
         case GC_GOAL: 
            reward = goalReward; 
            break;

         case GC_WITH_KEEPER_0: 
         case GC_WITH_KEEPER_1: 
         case GC_WITH_KEEPER_2: 
         case GC_WITH_KEEPER_3: 
            reward = passReward; 
            break;

         case GC_WITH_TAKER_0: 
         case GC_WITH_TAKER_1: 
         case GC_WITH_TAKER_2: 
         case GC_WITH_TAKER_3: 
            reward = ballTakenReward; 
            break;

         case GC_BALL_OUT_OF_PLAY: 
            reward = ballOutOfPlayReward;
            break;

         case GC_CAUGHT_BY_GOALIE:
            reward = goalieCatchReward;
            break;

         default: 
            gameConditionValid = false; 
            break;
      }

      if(gameConditionValid)
      {
         if (m_policy == 1)
            SA->update(state, action, reward, 1.0, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
         else
            SBBA->update(state, action, reward, 1.0, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
      }

      /*
         file.open("beta.txt", ios::out | ios :: app);
         file << endl << "Cycle: " << WM->getCurrentCycle() << endl  << "My Number: " << WM->getPlayerNumber() << endl;
         file << "Inside KP: " << endl;
         for(int i = 0; i < 17; i++)
         file << " " << state[i];

         file << " " << action << " ";
         file << gameCondition << endl;
         file.close();
         */
      WM->resetNewTrainerMessageHeard();  
   }	


   if ( WM->isNewEpisode())
   {
      m_episodeCount++;
      if (m_policy == 1)
         SA->endEpisode(reward, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
      else
         SBBA->endEpisode(reward, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());

      WM->setNewEpisode( false );
      WM->setLastAction( UnknownIntValue );

      m_episodeNumber++;
      m_timeStartEpisode = WM->getCurrentTime();
      //if((dumpMode) && ((m_episodeNumber % 200) == 0))
      //{
      //	SA->saveWeights(m_dumpWeightsFile);
      //	
      //	fstream file;
      //	file.open(m_dumpLogFile, ios::out);
      //	file << m_episodeNumber << endl;
      //	file.close();
      //}

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

SoccerCommand SGType1Keeper::keeperWithBall()
{
   double state[ MAX_STATE_VARS ];
   int action;

   int myAction = -1;
   int suggestedAction = -1;


   if ( WM->SGkeeperStateVars( state ) > 0 ) 
   { // if we can calculate state vars
      if( WM->getTimeLastAction() == WM->getCurrentCycle() - 1 && WM->getLastAction() > 0 ) 
      {   // if we were in the middle of a pass last cycle
         action = WM->getLastAction();         // then we follow through with it
      }
      else
      {
         //start = clock();
         if (m_policy == 1)
            action = SA->selectAction(state,WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
         else
            action = SBBA->selectAction(state, WM->getCurrentTime().getTime() - m_timeStartEpisode.getTime());
         //cout << "selectActionTime: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;

         SoccerCommand soc;
         char *yellMessage = WM->shootgoalTaskMakeStringToSay(state, action);

         soc = communicate(yellMessage);
         //Don't communicate twice in the same cycle: should never happen, but just in case!
         if(WM->getCurrentCycle() != WM->timeLastSayMessageSent() + 1)
         {
            ACT->putCommandInQueue(soc);
            /*
               fstream file;
               file.open("beta.txt", ios::out | ios::app);
               file << "Cycle: " << WM->getCurrentCycle() << " Player " << WM->getPlayerNumber() << " sending: " << yellMessage << endl;
               file.close();
               */		
            WM->setTimeLastSayMessageSent(WM->getCurrentCycle());

         }

         if((strlen(m_stateLogFile) > 9))///////Bad hack
         {

            fstream file;
            file.open(m_stateLogFile, ios::out | ios::app);
            double dummyState[100];
            int numStates = WM->SGkeeperStateVars( dummyState );		

            file << m_episodeNumber << "\t";
            for(int i = 0; i < numStates; i++)
            {
               file << state[i] << "\t";
               //fprintf(file, "%lf", state[i]);

               //char buffer[100];
               //double d = state[i];
               //sprintf(buffer, "%lf", d);
               //sscanf(buffer, "%lf", &d);
               //state[i] = d;

            }	

            file << action << "\t";//Action taken
            file << "ABRACADABRA"; 
            file << suggestedAction;//Action suggested--valid only in transfer mode (otherwise -1)

            file << endl;

            file.close();

            //if(action != SA->bestAction(state))
            //cout << "ALPHABETAGAMMADELTA" << endl;

         }
      }

      WM->setLastAction( action );

   }
   else 
   { // if we don't have enough info to calculate state vars
      action = ACTION_SG_DRIBBLE_NORMAL;  // Default 
      LogDraw.logText( "state", VecPosition( 35, 25 ),
            "clueless",
            1, COLOR_RED );
   }

   return interpretKeeperAction(action);
}

