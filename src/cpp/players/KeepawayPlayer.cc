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

#include "KeepawayPlayer.h"
#include "Parse.h"
#include "SayMsgEncoder.h"
#include <cstring>

extern LoggerDraw LogDraw;

KeepawayPlayer::KeepawayPlayer( PolicyAgent  *pa, ActHandler *act, WorldModel *wm, 
				ServerSettings *ss, PlayerSettings *ps,
				char* strTeamName, int iNumKeepers, int iNumTakers,
				bool isGoalie,
				double dVersion, int startEpisode, int iReconnect, int iStopAfter, int iPolicy )

{
  m_stopAfter = iStopAfter;
  m_policy = iPolicy;
  m_episodeCount = 0;
  char str[MAX_MSG];
  
  PA            = pa;
  ACT           = act;
  WM            = wm;
  SS            = ss;
  PS            = ps;
  bContLoop     = true;
  WM->setTeamName( strTeamName );
  WM->setNumKeepers( iNumKeepers );
  WM->setNumTakers( iNumTakers );
  WM->setIsGoalie(isGoalie); 
  WM->setNewEpisode( false );
  WM->setLastAction( UnknownIntValue );
  m_timeLastSay = -5;
  
  WM->resetNewTrainerMessageHeard();
  m_episodeNumber = startEpisode;

  // create initialisation string
  if( iReconnect != -1 )
    sprintf( str, "(reconnect %s %d)", strTeamName, iReconnect );
  else
  {
  	if(isGoalie)
    		sprintf( str, "(init %s (version %f) (goalie))", strTeamName, dVersion );
    	else
		sprintf( str, "(init %s (version %f))", strTeamName, dVersion );
  }
  
  ACT->sendMessage( str );
  
}

/*! This is the main loop of the agent. This method calls the update methods
    of the world model after it is indicated that new information has arrived.
    After this, the correct main loop of the player type is called, which
    puts the best soccer command in the queue of the ActHandler. */
void KeepawayPlayer::mainLoop( )
{
  Timing timer;

  // wait for new information from the server
  // cannot say bContLoop=WM->wait... since bContLoop can be changed elsewhere
  if(  WM->waitForNewInformation() == false )
    bContLoop =  false;

  while( bContLoop && (m_stopAfter == -1 || m_episodeCount <= m_stopAfter))     // as long as server alive
  {
    Log.logWithTime( 3, "  start update_all" );
    Log.setHeader( WM->getCurrentCycle() );
    LogDraw.setTime( WM->getCurrentCycle() );

    if( WM->updateAll( ) == true )
    {
      timer.restartTime();
      SoccerCommand soc;

/*      if ( WM->getSide() == SIDE_LEFT )
	soc = keeper();
      else
	soc = taker();
*/

	soc = player(m_episodeCount);
	
      if( shallISaySomething() == true )           // shall I communicate
        {
          m_timeLastSay = WM->getCurrentTime();
          char strMsg[MAX_SAY_MSG];
	  makeSayMessage( soc, strMsg );
          if( strlen( strMsg ) != 0 )
            Log.log( 600, "send communication string: %s", strMsg );
          WM->setCommunicationString( strMsg );
        }
      Log.logWithTime( 3, "  determined action; waiting for new info" );
      // directly after see message, will not get better info, so send commands
      if( WM->getTimeLastSeeMessage() == WM->getCurrentTime() ||
          (SS->getSynchMode() == true && WM->getRecvThink() == true ))
      {
        Log.logWithTime( 3, "  send messages directly" );
        ACT->sendCommands( );
        Log.logWithTime( 3, "  sent messages directly" );
        if( SS->getSynchMode() == true  )
        {
          WM->processRecvThink( false );
          ACT->sendMessageDirect( "(done)" );
        }
      }
    }  
    else
      Log.logWithTime( 3, "  HOLE no action determined; waiting for new info");

    int iIndex;
    double dConfThr = PS->getPlayerConfThr();
    char buffer[128];
    for( ObjectT o = WM->iterateObjectStart( iIndex, OBJECT_SET_TEAMMATES, dConfThr);
  	 o != OBJECT_ILLEGAL;
  	 o = WM->iterateObjectNext ( iIndex, OBJECT_SET_TEAMMATES, dConfThr ) ) {
      LogDraw.logCircle( "Players", WM->getGlobalPosition( o ), 1.6, 80,
			 false,
			 COLOR_ORANGE, WM->getConfidence( o ) );
      sprintf( buffer, "%d", SoccerTypes::getIndex( o ) + 1 );
      LogDraw.logText( "Players", WM->getGlobalPosition( o ),
		       buffer,
		       80, COLOR_ORANGE );
    }
    // Me
    ObjectT o = WM->getAgentObjectType();
    LogDraw.logCircle( "Players", WM->getGlobalPosition( o ), 1.6, 81,
		       false,
		       COLOR_PURPLE, WM->getConfidence( o ) );
    sprintf( buffer, "%d", SoccerTypes::getIndex( o ) + 1 );
    LogDraw.logText( "Players", WM->getGlobalPosition( o ),
		     buffer,
		     81, COLOR_PURPLE );
    for( ObjectT o = WM->iterateObjectStart( iIndex, OBJECT_SET_OPPONENTS, dConfThr);
  	 o != OBJECT_ILLEGAL;
  	 o = WM->iterateObjectNext ( iIndex, OBJECT_SET_OPPONENTS, dConfThr ) ) {
      LogDraw.logCircle( "Players", WM->getGlobalPosition( o ), 1.6, 80,
			 false,
			 COLOR_PINK, WM->getConfidence( o ) );
      sprintf( buffer, "%d", SoccerTypes::getIndex( o ) + 1 );
      LogDraw.logText( "Players", WM->getGlobalPosition( o ),
		       buffer,
		       80, COLOR_PINK );
    }

    Log.logWithTime( 604, "time for action: %f", timer.getElapsedTime()*1000 );
           
    // wait for new information from the server cannot say
    // bContLoop=WM->wait... since bContLoop can be changed elsewhere
    if(  WM->waitForNewInformation() == false )
        bContLoop =  false;
  }

  // shutdow, print hole and number of players seen statistics
  printf("Shutting down player %d\n", WM->getPlayerNumber() );
  printf("   Number of holes: %d (%f)\n", WM->iNrHoles,
                         ((double)WM->iNrHoles/WM->getCurrentCycle())*100 );
  printf("   Teammates seen: %d (%f)\n", WM->iNrTeammatesSeen,
                         ((double)WM->iNrTeammatesSeen/WM->getCurrentCycle()));
  printf("   Opponents seen: %d (%f)\n", WM->iNrOpponentsSeen,
                         ((double)WM->iNrOpponentsSeen/WM->getCurrentCycle()));

}


/********************** SAY **************************************************/

/*!This method determines whether a player should say something.
   \return bool indicating whether the agent should say a message */
bool KeepawayPlayer::shallISaySomething()
{
  bool        bReturn;

  bReturn  = ((WM->getCurrentTime() - m_timeLastSay) >= SS->getHearDecay());
  bReturn  &= (WM->getCurrentCycle() > 0 );

  return bReturn;
}

void KeepawayPlayer::makeSayMessage( SoccerCommand soc, char * strMsg )
{
  VecPosition posBall = WM->getGlobalPosition( OBJECT_BALL );
  VecPosition velBall = WM->getGlobalVelocity( OBJECT_BALL );
  int iDiff = 0;
  SayMsgEncoder myencoder;

  VecPosition posBallPred;
  WM->predictBallInfoAfterCommand( soc, &posBallPred );
  VecPosition posAgentPred = WM->predictAgentPosAfterCommand( soc );

  // If we have good information about the ball
  if( ( WM->getTimeChangeInformation(OBJECT_BALL) == WM->getCurrentTime() &&
	WM->getRelativeDistance( OBJECT_BALL ) < 20.0 &&
	WM->getTimeLastSeen( OBJECT_BALL ) == WM->getCurrentTime() )
      ||
      (
       WM->getRelativeDistance( OBJECT_BALL ) < SS->getVisibleDistance() &&
       WM->getTimeLastSeen( OBJECT_BALL ) == WM->getCurrentTime()  
       )
      ||
      (
       WM->getRelativeDistance( OBJECT_BALL ) < SS->getMaximalKickDist() &&
       posBallPred.getDistanceTo( posAgentPred ) > SS->getMaximalKickDist() 
       )
      ) 
  {
    // If we are kicking the ball 
    if( WM->getRelativeDistance( OBJECT_BALL ) < SS->getMaximalKickDist() )
    {
      // if kick and a pass
      if( soc.commandType == CMD_KICK )
      {
	WM->predictBallInfoAfterCommand( soc, &posBall, &velBall );
	VecPosition posAgent = WM->predictAgentPos( 1, 0 );
	if( posBall.getDistanceTo( posAgent ) > SS->getMaximalKickDist() + 0.2 )
	  iDiff = 1;
      }
      
      if( iDiff == 0 )
      {
	posBall = WM->getGlobalPosition( OBJECT_BALL );
	velBall.setVecPosition( 0, 0 );
      }
    }

    LogDraw.logCircle( "ball sending", posBall,
		       1.1, 90, false, COLOR_BLUE );
    
    myencoder.add( new BallInfo( posBall.getX(), posBall.getY(),
				 velBall.getX(), velBall.getY(), 1 - iDiff ) ); 
  }

  // Find closest opponent that was seen this cycle
  int numT = WM->getNumTakers();
  ObjectT T[ numT ];
  int numSeen = 0;
  for ( int i = 0; i < numT; i++ ) {
    T[ numSeen ] = SoccerTypes::getOpponentObjectFromIndex( i );
    if ( WM->getRelativeDistance( T[ numSeen ] ) < SS->getVisibleDistance() &&
	 WM->getTimeLastSeen( T[ numSeen ] ) == WM->getCurrentTime() )
      numSeen++;  // store this opponent if we just saw him
  }
  WM->sortClosestTo( T, numSeen, WM->getAgentObjectType() ); 

  if ( numSeen > 0 ) { // add closest
    VecPosition posOpp = WM->getGlobalPosition( T[ 0 ] );
    myencoder.add( new OppPos( SoccerTypes::getIndex( T[ 0 ] ) + 1,
			       posOpp.getX(), posOpp.getY(), 1 ) );
  }

  if ( myencoder.getSize() <= 7 &&  // if there is room
       WM->getConfidence( WM->getAgentObjectType() ) > PS->getPlayerHighConfThr() ) {
    myencoder.add( new OurPos( posAgentPred.getX(), posAgentPred.getY() ) );
  }

  strcpy( strMsg, myencoder.getEncodedStr().c_str() );
  myencoder.clear();
}

SoccerCommand KeepawayPlayer::interpretKeeperAction( int action )
{
  SoccerCommand soc;
  
  switch(action)
  {
	case ACTION_KWY_DRIBBLE_NORMAL://Dribble away from takers
		soc = holdBall();//KWYdribbleNormal();
		//soc = KWYdribbleNormal();
                break;
	
	case ACTION_KWY_PASS_TO_K_2://Pass to K_2
		soc = KWYpassToTeammate(1);
		break;
		
	case ACTION_KWY_PASS_TO_K_3://Pass to K_3
		soc = KWYpassToTeammate(2);
		break;
		
	case ACTION_KWY_PASS_TO_K_4://Pass to K_4
		soc = KWYpassToTeammate(3);
		break;

        case ACTION_KWY_PASS_TO_K_5://Pass to K_4
                soc = KWYpassToTeammate(4);
                break;
	
	default:
		soc = CMD_ILLEGAL;
		break;	
  }
  
  ACT->putCommandInQueue(soc);
  return soc;
}

SoccerCommand KeepawayPlayer::keeperSupport( ObjectT fastest )
{
  SoccerCommand soc;

  int iCycles = WM->predictNrCyclesToObject( fastest, OBJECT_BALL );
  VecPosition posPassFrom = WM->predictPosAfterNrCycles( OBJECT_BALL, iCycles );
  LogDraw.logCircle( "BallPredict", posPassFrom, 1, 70, true, COLOR_BROWN );
  
  soc = KWYgetOpen();	
  
  ObjectT lookObject = chooseLookObject( 0.97 );

  char buffer[128];
  LogDraw.logText( "lookObject", VecPosition( 25, -25 ), 
		   SoccerTypes::getObjectStr( buffer, lookObject ), 100, COLOR_WHITE );

  ACT->putCommandInQueue( soc );
  ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
  
  return soc;
}

ObjectT KeepawayPlayer::chooseLookObject( double ballThr )
{
  if ( WM->getConfidence( OBJECT_BALL ) < ballThr )
    return OBJECT_BALL;

  ObjectT objLeast = OBJECT_ILLEGAL;
  double confLeast = 1.1;
  for ( int i = 0; i < WM->getNumKeepers(); i++ ) {
    ObjectT obj = SoccerTypes::getTeammateObjectFromIndex( i );
    if ( obj != WM->getAgentObjectType() ) {
      double conf = WM->getConfidence( obj );
      if ( conf < confLeast ) {
	confLeast = conf;
	objLeast = obj;
      }
    }
  }

  return objLeast;
}

//Utility methods for Keepaway Task
VecPosition KeepawayPlayer::KWYbestPointToMove()
/*
	It is assumed that this method is used by a non-K1 keeper to get open.
	It is further assumed that there are exactly 4 keepers. This method ensures
	that they always stay approximately in a square formation.
*/
{

  	int numK = WM->getNumKeepers();//Must be 4
	ObjectT K[ numK ];

	for ( int i = 0; i < numK; i++ )
    		K[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );
		
	
	double WB_dist_to_K[ numK ];
  	ObjectT K1 = WM->getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL );
	
	bool dummyBool = WM->sortClosestTo( K, numK, K1, WB_dist_to_K );

	VecPosition K1Pos = WM->getGlobalPosition(K1);
	VecPosition myPos = WM->getGlobalPosition(WM->getAgentObjectType());
	
	const int unknown = 0;
	const int rightBottom = 1;
	const int rightTop = 2;
	const int leftTop = 3;
	const int leftBottom = 4;
	
	double squareXSide, squareYSide;

	int K1Orientation = unknown;
	int myOrientation = unknown;

	VecPosition point1, point2, point3, point4;
	VecPosition posToMove;
	
	VecPosition playerCentre = K1Pos + WM->getGlobalPosition(K[1]) + WM->getGlobalPosition(K[2]) + WM->getGlobalPosition(K[3]);
	playerCentre.setX(playerCentre.getX() / 4.0);
	playerCentre.setY(playerCentre.getY() / 4.0);
	
	squareXSide = 10.0;
	squareYSide = 10.0;
	
	
	if(K1Pos.getX() <= playerCentre.getX())
	{
		if(K1Pos.getY() <= playerCentre.getY())
		{
			K1Orientation = leftTop;
		}
		else
		{
			K1Orientation = leftBottom;
		}
	}
	else
	{
		if(K1Pos.getY() <= playerCentre.getY())
		{
			K1Orientation = rightTop;
		}
		else
		{
			K1Orientation = rightBottom;
		}
	}	
		
	if(myPos.getX() <= playerCentre.getX())
	{
		if(myPos.getY() <= playerCentre.getY())
		{
			myOrientation = leftTop;
		}
		else
		{
			myOrientation = leftBottom;
		}
	}
	else
	{
		if(myPos.getY() <= playerCentre.getY())
		{
			myOrientation = rightTop;
		}
		else
		{
			myOrientation = rightBottom;
		}
	}	
			
		
	
	if(K1Orientation == rightBottom)
	{
		point1 = VecPosition(K1Pos.getX(), K1Pos.getY() - squareYSide);
		point2 = VecPosition(K1Pos.getX() - squareXSide, K1Pos.getY() - squareYSide);
		point3 = VecPosition(K1Pos.getX() - squareXSide, K1Pos.getY());
	}
	else if(K1Orientation == rightTop)
	{
		point1 = VecPosition(K1Pos.getX() - squareXSide, K1Pos.getY());
		point2 = VecPosition(K1Pos.getX() - squareXSide, K1Pos.getY() + squareYSide);
		point3 = VecPosition(K1Pos.getX(), K1Pos.getY() + squareYSide);
	}
	else if(K1Orientation == leftTop)
	{
		point1 = VecPosition(K1Pos.getX(), K1Pos.getY() + squareYSide);
		point2 = VecPosition(K1Pos.getX() + squareXSide, K1Pos.getY() + squareYSide);
		point3 = VecPosition(K1Pos.getX() + squareXSide, K1Pos.getY());
	}
	else if(K1Orientation == leftBottom)
	{
		point1 = VecPosition(K1Pos.getX() + squareXSide, K1Pos.getY());
		point2 = VecPosition(K1Pos.getX() + squareXSide, K1Pos.getY() - squareYSide);
		point3 = VecPosition(K1Pos.getX(), K1Pos.getY() - squareYSide);
	}
	else //K1Orientation unknown
	{
		//You will remain where you are.
	}			
		
	if(playerCentre.getX() < 0)
	{
		point1.setX(point1.getX() + 0.5);
		point2.setX(point2.getX() + 0.5);
		point3.setX(point3.getX() + 0.5);
	}
	else if(playerCentre.getX() > 0)
	{
		point1.setX(point1.getX() - 0.5);
		point2.setX(point2.getX() - 0.5);
		point3.setX(point3.getX() - 0.5);
	}
		
	if(playerCentre.getY() < 0)
	{
		point1.setY(point1.getY() + 0.5);
		point2.setY(point2.getY() + 0.5);
		point3.setY(point3.getY() + 0.5);
	}
	else if(playerCentre.getY() > 0)
	{
		point1.setY(point1.getY() - 0.5);
		point2.setY(point2.getY() - 0.5);
		point3.setY(point3.getY() - 0.5);
	}
	
	if(K1Orientation == rightBottom)
	{
		if(myOrientation == rightTop)
			posToMove = point1;
		else if(myOrientation == leftTop)
			posToMove = point2;
		else if(myOrientation == leftBottom)
			posToMove = point3;
		else
			posToMove = myPos;//This should ideally not happen	
	}	
	else if(K1Orientation == rightTop)
	{
		if(myOrientation == leftTop)
			posToMove = point1;
		else if(myOrientation == leftBottom)
			posToMove = point2;
		else if(myOrientation == rightBottom)
			posToMove = point3;
		else
			posToMove = myPos;//This should ideally not happen	
	}	
	else if(K1Orientation == leftTop)
	{
		if(myOrientation == leftBottom)
			posToMove = point1;
		else if(myOrientation == rightBottom)
			posToMove = point2;
		else if(myOrientation == rightTop)
			posToMove = point3;
		else
			posToMove = myPos;//This should ideally not happen	
	}
	else if(K1Orientation == leftBottom)
	{
		if(myOrientation == rightBottom)
			posToMove = point1;
		else if(myOrientation == rightTop)
			posToMove = point2;
		else if(myOrientation == leftTop)
			posToMove = point3;
		else
			posToMove = myPos;//This should ideally not happen	
	}
	else
	{
			posToMove = myPos;//This should ideally not happen	
	}	
	
	
	for(int i = 0; i < numK; i++)
	{
		if(K[i] == WM->getAgentObjectType())
			continue;
		
		VecPosition KiPos = WM->getGlobalPosition(K[i]);	
		if(posToMove.getDistanceTo(KiPos) < 5.0);
		{
			VecPosition tempRay = VecPosition(2.0, (posToMove - KiPos).getDirection(), POLAR);
			posToMove = posToMove + tempRay;	
		}
	}

	posToMove = KWYoptimiseBestPointToMoveTo(posToMove, 2.0);
	
	return posToMove;
}


VecPosition KeepawayPlayer::KWYoptimiseBestPointToMoveTo(VecPosition point, double dis)
{
	int granularity = 10;
	
	VecPosition tempPoint, bestPoint;
	double tempGoodness, bestGoodness;
	
	bestPoint = point;
	bestGoodness = KWYgetOpenGoodness(bestPoint);
	
	double pointX = point.getX();
	double pointY = point.getY();
	
	double gridSide = (2.0 * dis) / granularity;
	
	for(int i = 0; i <= granularity; i++)
		for(int j = 0; j <= granularity; j++)
		{
			tempPoint.setX(pointX - dis + (i * gridSide));
			tempPoint.setY(pointY - dis + (j * gridSide));
					
			if(tempPoint.getX() < -16.0) 
				tempPoint.setX(-16.0);
			if(tempPoint.getX() > 16.0) 
				tempPoint.setX(16.0);
			if(tempPoint.getY() < -16.0) 
				tempPoint.setY(-16.0);
			if(tempPoint.getY() > 16.0) 
				tempPoint.setY(16.0);

			tempGoodness = KWYgetOpenGoodness(tempPoint);
				
			if(tempGoodness > bestGoodness)
			{
				bestPoint = tempPoint;
				bestGoodness = tempGoodness;
			}	 
		}
		
	return bestPoint;
}

double KeepawayPlayer::KWYgetOpenGoodness(VecPosition point)
{

	ObjectT K1 = WM->getClosestInSetTo(OBJECT_SET_TEAMMATES, OBJECT_BALL);
	
	VecPosition K1Pos = WM->getGlobalPosition(K1);
	
	double minAngTClose = WM->minAngWithCloseT(K1Pos, point);
	
	double goodness = minAngTClose;
		
	return goodness; 
}

//Actions for Keepaway Task
SoccerCommand KeepawayPlayer::KWYpassToTeammate(int teammate)//player assumed keeper
{
    SoccerCommand soc;
    
    if(teammate == 0)
    	return CMD_ILLEGAL;
	
    int numK = WM->getNumKeepers();
    ObjectT K[ numK ];
    for ( int i = 0; i < numK; i++ )
      K[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );
	
    
    double WB_dist_to_K[ numK ];
    ObjectT K1 = WM->getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL );//Must be me!
    bool dummyBool = WM->sortClosestTo( K, numK, K1, WB_dist_to_K );
    
      
    VecPosition tmPos = WM->getGlobalPosition( K[teammate] );
    
    if(tmPos.getX() < -16.0)
    	tmPos.setX(-16.0);
    if(tmPos.getX() > 16.0)
    	tmPos.setX(16.0);
    if(tmPos.getY() < -16.0)
    	tmPos.setY(-16.0);
    if(tmPos.getY() > 16.0)
    	tmPos.setY(16.0);
    
    soc = directPass(tmPos, PASS_NORMAL);
    
    return soc;
}

SoccerCommand KeepawayPlayer::KWYdribbleNormal()
{
	SoccerCommand soc;
	
	VecPosition posToKick;
		
	VecPosition centre = VecPosition(0, 0);
	
        VecPosition myPos = WM->getGlobalPosition(WM->getAgentObjectType());
	
	double minDisTInCone = WM->KWYminDisToTInCone(myPos);

	int direction;
	AngDeg fanAngle;
	AngDeg startAngle;
	
	if(minDisTInCone > 14.0)
		fanAngle = 30.0;
	else if(minDisTInCone > 10.0) 
		fanAngle = 60.0;//120.0;
	else
		fanAngle = 90.0;//150.0;	
	
	startAngle = (centre - myPos).getDirection() - (fanAngle / 2.0);
	
	direction = 1;

	double dribbleRadius = 4.0;
	
	int numGrains = 60;
	VecPosition bestPoint = myPos;
	double bestGoodness = -1000.0;//WM->minDisToT(myPos);
	
	for(int i = 0; i <= numGrains; i++)
	{
		VecPosition tempRay = VecPosition(dribbleRadius, startAngle, POLAR);
		tempRay.rotate(direction * i * (fanAngle / numGrains));
		VecPosition tempPoint = myPos + tempRay;
		
		if(tempPoint.getX() < -16.0)
			continue;		
		if(tempPoint.getX() > 16.0)
			continue;		
		if(tempPoint.getY() < -16.0)
			continue;		
		if(tempPoint.getY() > 16.0)
			continue;		
		
		double tempGoodness;
		 
		tempGoodness = WM->minDisToT(tempPoint);
		
		if(tempGoodness > bestGoodness)
		{
			bestPoint = tempPoint;
			bestGoodness = tempGoodness;
		}
	}	
	
	if(bestPoint.getDistanceTo(myPos) == 0)//They are the same
		soc = holdBall();//Will not happen
	else
	{
		//soc = kickTo(bestPoint, 0.7);
                soc = dribble((bestPoint - myPos).getDirection(), DRIBBLE_WITHBALL);
	}

	return soc;
}

SoccerCommand KeepawayPlayer::KWYgetOpen()
{
	SoccerCommand soc;
	
	VecPosition ballPoint = WM->getGlobalPosition(OBJECT_BALL);
	VecPosition targetPoint;
	
	if((WM->getGlobalPosition(WM->getAgentObjectType())).getDistanceTo(ballPoint) > 25.0)
		targetPoint = ballPoint;
	else
		targetPoint = KWYbestPointToMove();
	
	
	if((WM->getGlobalPosition(WM->getAgentObjectType())).getDistanceTo(targetPoint) < 1.5)
		soc = turnBodyToPoint(WM->getGlobalPosition(OBJECT_BALL));
	else
		soc = moveToPos(targetPoint, 30.0, 1.0, false, 2);
		
	return soc;	

}

