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

#include "SGTaker.h"
#include "Parse.h"
#include "SayMsgEncoder.h"
#include <sstream>
#include <sys/stat.h>
#include <cstdlib>

inline bool fileExists(const char *fileName)
{
   ifstream infile(fileName);
   return infile.good();
}

extern LoggerDraw LogDraw;

SGTaker::SGTaker( SarsaAgent  *sa, ActHandler *act, WorldModel *wm, 
      ServerSettings *ss, PlayerSettings *ps,
      char* strTeamName, int id, int iNumKeepers, int iNumTakers,
      bool isGoalie,
      double dVersion, int startEpisode, char *stateLogFile, int iReconnect, int iStopAfter ):
   ShootgoalPlayer( sa, act, wm, ss, ps, strTeamName, id, iNumKeepers, iNumTakers, isGoalie, dVersion, startEpisode, iReconnect, iStopAfter )		
{
   m_id = id;
}


SoccerCommand SGTaker::player(int & episodeCount)
{
   //cout << "PlayMode " << WM->getPlayMode() << endl;
   SoccerCommand soc;

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

   if ( WM->isNewEpisode() )
   {
      episodeCount++;
      //cout << "Taker episodeCount " << episodeCount << endl;

      WM->setNewEpisode( false );
      WM->setLastAction( UnknownIntValue );

      m_episodeNumber++;
   }

   LogDraw.logCircle( "ball pos", WM->getBallPos(),
         1.1, 11, false, COLOR_SEASHELL, WM->getConfidence( OBJECT_BALL ) );

   //// Interactive /////////////////////////////////////////////////////////
   //// Maintain possession if you have the ball.
   //if (WM->isBallKickable() && WM->getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL) == WM->getAgentObjectType())
   //{
   //   ACT->putCommandInQueue( soc = holdBall() );
   //   return soc;
   //}
   //int m = -1;
   //int m_prev = -1;
   //VecPosition destination;
   //VecPosition myPosI = WM->getGlobalPosition(WM->getAgentObjectType());
   //destination = VecPosition(myPosI.getX(),myPosI.getY());
   //const int MAXLINE=102400;
   //ifstream inFile;
   //char inputFilename[80];
   //ostringstream oss;
   //oss.str("");
   //oss << "interactive/taker." << m_id;
   //sprintf( inputFilename, oss.str().c_str());
   //oss.str("");
   //if (fileExists(inputFilename) == false){
   //   soc = moveToPos(destination, 30, 5, false, 5);
   //   ACT->putCommandInQueue(soc);
   //   return soc;
   //}
   //inFile.open(inputFilename, ios::in);
   //string oneline;
   //while(getline(inFile, oneline)){ //read to last line
   //   m = atoi(oneline.c_str());
   //   m_prev=m;
   //}
   //inFile.close();
   //switch(m_prev)
   //{
   //   case 0: //UP
   //      myPosI = WM->getGlobalPosition(WM->getAgentObjectType());
   //      destination = VecPosition(myPosI.getX(),myPosI.getY()+10);
   //      break;
   //   case 1: //DOWN
   //      myPosI = WM->getGlobalPosition(WM->getAgentObjectType());
   //      destination = VecPosition(myPosI.getX(),myPosI.getY()-10);
   //      break;
   //   case 2: //LEFT
   //      myPosI = WM->getGlobalPosition(WM->getAgentObjectType());
   //      destination = VecPosition(myPosI.getX()-10,myPosI.getY());
   //      break;
   //   case 3: //RIGHT
   //      myPosI = WM->getGlobalPosition(WM->getAgentObjectType());
   //      destination = VecPosition(myPosI.getX()+10,myPosI.getY());
   //      break;
   //   default: //STAY
   //      myPosI = WM->getGlobalPosition(WM->getAgentObjectType());
   //      destination = VecPosition(myPosI.getX(),myPosI.getY());
   //      break;
   //} 
   //soc = moveToPos(destination, 30, 5, false, 5);
   //ACT->putCommandInQueue(soc);
   //return soc;
   ///////////////////////////////////////////////////////////////////////

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

   //Goalie policy
   if (WM->getIsGoalie())//This characterises the goalie
   {

      if((myPos.getDistanceTo(ballPos) < 0.9 * SS->getCatchableAreaL()) && ((ballPos.getX() < -PITCH_LENGTH / 2.0 + PENALTY_AREA_LENGTH) && (fabs(ballPos.getY()) < PENALTY_AREA_WIDTH / 2.0)))
      {
         soc = catchBall();

         if(soc.commandType != CMD_ILLEGAL)
         {	
            ACT->putCommandInQueue(soc);
            return soc;
         }
      }

      VecPosition dummyVecPos;

      VecPosition shootPoint;
      double dummyAng = WM->maxAngWithGoal(ballPos, &shootPoint);

      double ballMaxAngleToGoal = WM->maxAngWithGoal(ballPos, &dummyVecPos);
      double ballDisToGoal = WM->distanceToGoal(ballPos);

      double ballSpeed = WM->getBallSpeed();

      VecPosition ballPosNext = WM->predictPosAfterNrCycles(OBJECT_BALL, 4);
      VecPosition goalRightUpright = VecPosition(-PITCH_LENGTH / 2.0, SS->getGoalWidth() / 2.0 + 2.0);//The buffer is for safety
      VecPosition goalLeftUpright = VecPosition(-PITCH_LENGTH / 2.0, -SS->getGoalWidth() / 2.0 - 2.0);//The buffer is for safety
      double ballGoalAngle = ballPos.getAngleBetweenPoints(goalRightUpright, goalLeftUpright);

      bool ballIsHeadingForGoal = ballSpeed > 1.0;
      ballIsHeadingForGoal = ballIsHeadingForGoal && (ballPos.getAngleBetweenPoints(goalRightUpright, ballPosNext) <= ballGoalAngle);
      ballIsHeadingForGoal = ballIsHeadingForGoal && (ballPos.getAngleBetweenPoints(ballPosNext, goalLeftUpright) <= ballGoalAngle);

      bool ballInPenaltyArea = (ballPos.getX() < -PITCH_LENGTH / 2.0 + PENALTY_AREA_LENGTH) && (fabs(ballPos.getY()) < PENALTY_AREA_WIDTH / 2.0);

      //if((ballPosNext.getX() < -PITCH_LENGTH / 2.0 + PENALTY_AREA_LENGTH) && (fabs(ballPosNext.getY()) < PENALTY_AREA_WIDTH / 2.0))
      if((ballIsHeadingForGoal) || (ballInPenaltyArea && (WM->getAgentObjectType() == T[0])))
      {
         soc = intercept(true);

         if(soc.commandType != CMD_ILLEGAL)
         {
            ACT->putCommandInQueue(soc);
            ACT->putCommandInQueue(turnNeckToObject(OBJECT_BALL, soc));
            return soc;
         }
      }

      soc = defendGoalLine(2.0);//Defend the goal at a distance of 2.0m.
      if(soc.commandType != CMD_ILLEGAL)
      {
         ACT->putCommandInQueue(soc);
         return soc;
      }

      //This should never happen
      soc = moveToPos(PITCH_LENGTH / 2.0, 0);
      ACT->putCommandInQueue(soc);
      return soc;
   }

   // Maintain possession if you have the ball.
   if (WM->isBallKickable() && WM->getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL) == WM->getAgentObjectType()) 
   {
      ACT->putCommandInQueue( soc = holdBall() );
      return soc;
   }  

   ObjectT closestKeeperToGoal, keeperMostOpenForPass, keeperWithBall;

   closestKeeperToGoal = SoccerTypes::getOpponentObjectFromIndex(0);
   keeperMostOpenForPass = SoccerTypes::getOpponentObjectFromIndex(0);
   keeperWithBall = SoccerTypes::getOpponentObjectFromIndex(0);

   for(int i = 1; i < WM->getNumKeepers(); i++)
   {
      ObjectT currentPlayer = SoccerTypes::getOpponentObjectFromIndex(i);

      if(WM->distanceToGoal(WM->getGlobalPosition(currentPlayer)) < WM->distanceToGoal(WM->getGlobalPosition(closestKeeperToGoal)))
         closestKeeperToGoal = currentPlayer;

      if(WM->getGlobalPosition(currentPlayer).getDistanceTo(ballPos) < WM->getGlobalPosition(keeperWithBall).getDistanceTo(ballPos))
         keeperWithBall = currentPlayer;
   }	

   double maxAngle = -1000.0;
   keeperMostOpenForPass = keeperWithBall;//dummy
   for(int i = 0; i < WM->getNumKeepers(); i++)
   {
      ObjectT currentPlayer = SoccerTypes::getOpponentObjectFromIndex(i);

      VecPosition keeperWithBallPosition = WM->getGlobalPosition(keeperWithBall);
      VecPosition currentPlayerPosition = WM->getGlobalPosition(currentPlayer);

      if(currentPlayer == keeperWithBall)
         continue;

      if(WM->minAngWithCloseT(keeperWithBallPosition, currentPlayerPosition) > maxAngle)
      {
         maxAngle = WM->minAngWithCloseT(keeperWithBallPosition, currentPlayerPosition);
         keeperMostOpenForPass = currentPlayer;
      }		
   }	


   keeperWithBall = K[0];


   bool shouldIIntercept = (WM->getAgentObjectType() == T[0]) && (myPos.getDistanceTo(ballPos) <= 1.5 * WM->getGlobalPosition(K[0]).getDistanceTo(ballPos));

   VecPosition shootPoint;
   double dummyAng = WM->maxAngWithGoal(ballPos, &shootPoint);

   VecPosition aheadOfBall;
   aheadOfBall.setX(ballPos.getX() * 0.9 + shootPoint.getX() * 0.1);
   aheadOfBall.setY(ballPos.getY() * 0.9 + shootPoint.getY() * 0.1);


   if(WM->getAgentObjectType() == T[0])
   {
      if(shouldIIntercept)
         soc = intercept(false);
      else
         soc = moveToPos(aheadOfBall, 30, 5, false, 5);

      ACT->putCommandInQueue(soc);
      return soc;
   }	

   if((WM->getAgentIndex() == 0) || (WM->getAgentIndex() == 1))
   {
      ObjectT T1 = SoccerTypes::getTeammateObjectFromIndex(0);
      ObjectT T2 = SoccerTypes::getTeammateObjectFromIndex(1);

      if(WM->getGlobalPosition(T1).getDistanceTo(ballPos) < WM->getGlobalPosition(T2).getDistanceTo(ballPos))
      {
         if(T1 == WM->getAgentObjectType())
         {	
            if(shouldIIntercept)
               soc = intercept(false);
            else
               soc = moveToPos(aheadOfBall, 30, 5, false, 5);
         }
         else
            soc = markMostOpenOpponent(keeperWithBall);
      }
      else
      {
         if(T2 == WM->getAgentObjectType())
         {	
            if(shouldIIntercept)
               soc = intercept(false);
            else
               soc = moveToPos(aheadOfBall, 30, 5, false, 5);
         }
         else
            soc = markMostOpenOpponent(keeperWithBall);
      }

      ACT->putCommandInQueue(soc);
      ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
      return soc;		
   }


   for(int i = 0; i < numK; i++)
      for(int j = 0; j < numK - 1; j++)
      {
         double K1X = WM->getGlobalPosition(K[j]).getX();
         double K2X = WM->getGlobalPosition(K[j + 1]).getX();

         if(K1X > K2X)
         {
            ObjectT tempK = K[j];
            K[j] = K[j + 1];
            K[j + 1] = tempK;
         }
      }

   VecPosition rightK1Pos = WM->getGlobalPosition(K[0]);
   VecPosition rightK2Pos = WM->getGlobalPosition(K[1]);

   VecPosition topRightKPos, bottomRightKPos;
   if(rightK1Pos.getY() < rightK2Pos.getY())
   {
      topRightKPos= rightK1Pos;
      bottomRightKPos = rightK2Pos;
   }
   else
   {
      topRightKPos= rightK2Pos;
      bottomRightKPos = rightK1Pos;
   }


   rightK1Pos = topRightKPos;
   rightK2Pos = bottomRightKPos;		

   if(WM->getAgentIndex() == 2)
   {
      VecPosition shootPoint;
      double dummyAng = WM->maxAngWithGoal(rightK1Pos, &shootPoint);
      double destinationX, destinationY;
      VecPosition destination;

      if(rightK1Pos.getDistanceTo(WM->getGlobalPosition(keeperWithBall)) == 0)
      {
         soc = intercept(false);
      }
      else if(rightK2Pos.getDistanceTo(WM->getGlobalPosition(keeperWithBall)) == 0)
      {
         soc = intercept(false);

         destinationX = rightK2Pos.getX() * 0.3 + shootPoint.getX() * 0.7;
         destinationY = rightK2Pos.getY() * 0.3 + shootPoint.getY() * 0.7;
         destination = VecPosition(destinationX, destinationY);

         soc = moveToPos(destination, 30, 5, false, 5);
      }
      else
      {
         destinationX = rightK1Pos.getX() * 0.7 + shootPoint.getX() * 0.3;
         destinationY = rightK1Pos.getY() * 0.7 + shootPoint.getY() * 0.3;
         destination = VecPosition(destinationX, destinationY);

         soc = moveToPos(destination, 30, 5, false, 5);
      }


      ACT->putCommandInQueue(soc);
      ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
      return soc;		
   }

   if(WM->getAgentIndex() == 3)
   {

      VecPosition shootPoint;
      double dummyAng = WM->maxAngWithGoal(rightK1Pos, &shootPoint);
      double destinationX, destinationY;
      VecPosition destination;

      if(rightK2Pos.getDistanceTo(WM->getGlobalPosition(keeperWithBall)) == 0)
      {
         soc = intercept(false);
      }
      else if(rightK1Pos.getDistanceTo(WM->getGlobalPosition(keeperWithBall)) == 0)
      {
         soc = intercept(false);

         destinationX = rightK1Pos.getX() * 0.3 + shootPoint.getX() * 0.7;
         destinationY = rightK1Pos.getY() * 0.3 + shootPoint.getY() * 0.7;
         destination = VecPosition(destinationX, destinationY);

         soc = moveToPos(destination, 30, 5, false, 5);

      }
      else
      {
         destinationX = rightK2Pos.getX() * 0.7 + shootPoint.getX() * 0.3;
         destinationY = rightK2Pos.getY() * 0.7 + shootPoint.getY() * 0.3;
         destination = VecPosition(destinationX, destinationY);

         soc = moveToPos(destination, 30, 5, false, 5);
      }

      ACT->putCommandInQueue(soc);
      ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
      return soc;
   }

   //  if ( WM->isNewEpisode() )
   //  {
   //        episodeCount++;
   //        cout << "Taker episodeCount " << episodeCount << endl;
   //
   //        WM->setNewEpisode( false );
   //        WM->setLastAction( UnknownIntValue );
   //
   //        m_episodeNumber++;
   //  }


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
