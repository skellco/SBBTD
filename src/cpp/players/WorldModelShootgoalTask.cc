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

#include "WorldModel.h"

int WorldModel::getNumKeepers()
{
	return m_numKeepers;
}

void WorldModel::setNumKeepers( int iNum )
{
	m_numKeepers = iNum;
}

int WorldModel::getNumTakers()
{
	return m_numTakers;
}

void WorldModel::setNumTakers( int iNum )
{
	m_numTakers = iNum;
}

double WorldModel::congestion( VecPosition pos, bool considerMe ) 
{
	double congest = 0;
	if ( considerMe && pos != getAgentGlobalPosition() ) 
		congest = 1 / getAgentGlobalPosition().getDistanceTo( pos );

	VecPosition playerPos;

	int iIndex;
	for(ObjectT obj = iterateObjectStart( iIndex, OBJECT_SET_TEAMMATES );
			obj != OBJECT_ILLEGAL;
			obj = iterateObjectNext ( iIndex, OBJECT_SET_TEAMMATES ) ) {
		if ( ( playerPos = getGlobalPosition( obj ) ) != pos )
			if ( obj != getAgentObjectType() )
				/* Don't want to count a player in its own congestion measure */
				congest += 1.0 / playerPos.getDistanceTo( pos );
	}

	ObjectT K1 = getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL );
	VecPosition K1Pos = getGlobalPosition(K1);

	congest -= 5.0 / K1Pos.getDistanceTo(pos);


	//int iIndex;
	for( ObjectT obj = iterateObjectStart( iIndex, OBJECT_SET_OPPONENTS );
			obj != OBJECT_ILLEGAL;
			obj = iterateObjectNext ( iIndex, OBJECT_SET_OPPONENTS ) ) {
		if ( ( playerPos = getGlobalPosition( obj ) ) != pos )
			if ( obj != getAgentObjectType() )
				/* Don't want to count a player in its own congestion measure */
				congest += 5.0 / playerPos.getDistanceTo( pos );
	}

	congest -= 400.0 /distanceToGoal(pos);

	return congest;
}

void WorldModel::resetEpisode()
{
	LogDraw.logText( "episode", VecPosition( 0, 0 ), "Reset", 
			40, COLOR_WHITE );
	Ball.setTimeLastSeen( -1 );
	for ( int i = 0; i < MAX_TEAMMATES; i++ )
		Teammates[i].setTimeLastSeen( -1 );
	for ( int i = 0; i < MAX_OPPONENTS; i++ )
		Opponents[i].setTimeLastSeen( -1 );
	for ( int i = 0; i < MAX_TEAMMATES+MAX_OPPONENTS; i++ )
		UnknownPlayers[i].setTimeLastSeen( -1 );
	iNrUnknownPlayers = 0;
	for ( int i = 0; i < MAX_FLAGS; i++ )
		Flags[i].setTimeLastSeen( -1 );
	for ( int i = 0; i < MAX_LINES; i++ )
		Lines[i].setTimeLastSeen( -1 );

	setNewEpisode( true );
}

void WorldModel::setNewEpisode( bool bNewEp )
{
	m_newEpisode = bNewEp;
}

bool WorldModel::isNewEpisode()
{
	return m_newEpisode;
}

void WorldModel::setLastAction( int iAction )
{
	m_lastAction = iAction;
	m_timeLastAction = 
		( iAction == UnknownIntValue ) ? UnknownTime : getCurrentCycle();
}

int WorldModel::getLastAction()
{
	return m_lastAction;
}

int WorldModel::getTimeLastAction()
{
	return m_timeLastAction;
}

int WorldModel::SGkeeperStateVars( double state[] )
{
	ObjectT K1 = getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL );
	/*
	   if ( !SoccerTypes::isTeammate( K1 ) )
	   return 0; // maybe change to != agentObject
	 */

	int numK = getNumKeepers();
	int numT = getNumTakers();

	VecPosition posK1 = getGlobalPosition(K1);

	ObjectT K[ numK ];
	for ( int i = 0; i < numK; i++ )
		K[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );

	ObjectT T[ numT ];
	for ( int i = 0; i < numT; i++ )
		T[ i ] = SoccerTypes::getOpponentObjectFromIndex( i );

	double WB_dist_to_K[ numK ];
	if ( !sortClosestTo( K, numK, K1, WB_dist_to_K ) )
		return 0;

	double WB_dist_to_T[ numT ];
	if ( !sortClosestTo( T, numT, K1, WB_dist_to_T ) )
		return 0;

	///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////
	VecPosition posBall = getGlobalPosition(OBJECT_BALL);
	/////////////////////////

	int j = 0;
        //dist to  teammates #3
	for(int i = 1; i < numK; i++){
		state[j++] = posK1.getDistanceTo(getGlobalPosition(K[i]));
		//cout << j-1 << " Adist" << endl;
	}
        //dist to closest opponent for all teammates #3
	for(int i = 1; i < numK; i++){
		state[j++] = minDisToT(getGlobalPosition(K[i]));
		//cout << j-1 << " Bdist" << endl;
	}
        //for all teammates, min angle from O1 between teammate and opponent #3
	for(int i = 1; i < numK; i++){
		state[j++] = minAngWithCloseT(posK1, getGlobalPosition(K[i]));
		//cout << j-1 << " Cang" << endl;
	}
        //min dist to any appoenent in the dribble cone #1
	state[j++] = SGminDisToTInCone(posK1);	
	//cout << j-1 << " Ddist" << endl;
        //dist to closest opponent #1
	state[j++] = minDisToT(posK1);	
	//cout << j-1 << " Edist" << endl;
        //distance to goal for each offense player #4
	for(int i = 0; i < numK; i++){
		state[j++] = distanceToGoal(getGlobalPosition(K[i]));
		//cout << j-1 << " Fdist" << endl;
	}
	VecPosition dummyVecPos;	
        //max_goal_ang(O1) #1	
	state[j++] = maxAngWithGoal(posK1, &dummyVecPos);
	//cout << j-1 << " Gang" << endl;
        // dist to goal #1
	state[j++] = distanceToGoalie(posK1);
	//cout << j-1 << " Hdist" << endl;

	/*
	   int j = 0;

	   for(int i = 1; i < numK; i++)
	   state[j++] = posBall.getDistanceTo(getGlobalPosition(K[i]));

	   for(int i = 1; i < numK; i++)
	   state[j++] = minDisToT(getGlobalPosition(K[i]));

	   for(int i = 1; i < numK; i++)
	   state[j++] = minAngWithCloseT(posBall, getGlobalPosition(K[i]));

	   state[j++] = SGminDisToTInCone(posBall);	

	   state[j++] = minDisToT(posBall);	

	   for(int i = 0; i < numK; i++)
	   state[j++] = distanceToGoal(getGlobalPosition(K[i]));

	   VecPosition dummyVecPos;		
	   state[j++] = maxAngWithGoal(posBall, &dummyVecPos);

	   state[j++] = distanceToGoalie(posBall);
	 */

	//cout << "SGkeeperStateVars";
	//for (int s = 0; s <= j; s++)
	//  cout << " " << state[s];
	//cout << endl;
	return j;
}

/*****************************************************************************************/

int WorldModel::SGkeeperStateVarsMin( double state[] )
{
	ObjectT K1 = getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL );
	/*
	   if ( !SoccerTypes::isTeammate( K1 ) )
	   return 0; // maybe change to != agentObject
	 */

	int numK = getNumKeepers();
	int numT = getNumTakers();

	VecPosition posK1 = getGlobalPosition(K1);

	ObjectT K[ numK ];
	for ( int i = 0; i < numK; i++ )
		K[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );

	ObjectT T[ numT ];
	for ( int i = 0; i < numT; i++ )
		T[ i ] = SoccerTypes::getOpponentObjectFromIndex( i );

	double WB_dist_to_K[ numK ];
	if ( !sortClosestTo( K, numK, K1, WB_dist_to_K ) )
		return 0;

	double WB_dist_to_T[ numT ];
	if ( !sortClosestTo( T, numT, K1, WB_dist_to_T ) )
		return 0;

	///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////
	VecPosition posBall = getGlobalPosition(OBJECT_BALL);
	/////////////////////////

	int j = 0;
        //dist to  teammates #3
	for(int i = 1; i < numK; i++){
		state[j++] = posK1.getDistanceTo(getGlobalPosition(K[i]));
		//cout << j-1 << " Adist" << endl;
	}
        //dist to closest opponent for all teammates #3
	for(int i = 1; i < numK; i++){
		state[j++] = minDisToT(getGlobalPosition(K[i]));
		//cout << j-1 << " Bdist" << endl;
	}
        //for all teammates, min angle from O1 between teammate and opponent #3
	for(int i = 1; i < numK; i++){
		state[j++] = minAngWithCloseT(posK1, getGlobalPosition(K[i]));
		//cout << j-1 << " Cang" << endl;
	}
        //min dist to any appoenent in the dribble cone #1
	state[j++] = SGminDisToTInCone(posK1);	
	//cout << j-1 << " Ddist" << endl;
        //dist to closest opponent #1
	state[j++] = minDisToT(posK1);	
	//cout << j-1 << " Edist" << endl;
        //distance to goal for each offense player #4
	for(int i = 0; i < numK; i++){
		state[j++] = distanceToGoal(getGlobalPosition(K[i]));
		//cout << j-1 << " Fdist" << endl;
	}
	VecPosition dummyVecPos;	
        //max_goal_ang(O1) #1	
	state[j++] = maxAngWithGoal(posK1, &dummyVecPos);
	//cout << j-1 << " Gang" << endl;
        // dist to goal #1
	state[j++] = distanceToGoalie(posK1);
	//cout << j-1 << " Hdist" << endl;

	/*
	   int j = 0;

	   for(int i = 1; i < numK; i++)
	   state[j++] = posBall.getDistanceTo(getGlobalPosition(K[i]));

	   for(int i = 1; i < numK; i++)
	   state[j++] = minDisToT(getGlobalPosition(K[i]));

	   for(int i = 1; i < numK; i++)
	   state[j++] = minAngWithCloseT(posBall, getGlobalPosition(K[i]));

	   state[j++] = SGminDisToTInCone(posBall);	

	   state[j++] = minDisToT(posBall);	

	   for(int i = 0; i < numK; i++)
	   state[j++] = distanceToGoal(getGlobalPosition(K[i]));

	   VecPosition dummyVecPos;		
	   state[j++] = maxAngWithGoal(posBall, &dummyVecPos);

	   state[j++] = distanceToGoalie(posBall);
	 */

	//cout << "SGkeeperStateVars";
	//for (int s = 0; s <= j; s++)
	//  cout << " " << state[s];
	//cout << endl;
	return j;
}

// Greg:
// This really doesn't belong here,
// because it is related to a specific 
// learner.  I don't know exactly
// where to put it, though because
// I want to keep the LinearSarsa
// class generic.

// Yaxin: changed from keeperTileWidths to keeperResolutions and keeperRanges,

int WorldModel::SGkeeperStateRangesAndResolutions( double ranges[], 
		double minValues[], 
		double resolutions[], 
		int numK, int numT )
{
	if ( numK < 3 ) {
		cerr << "keeperTileWidths: num keepers must be at least 3, found: " 
			<< numK << endl;
		return 0;
	}

	if ( numT < 2 ) {
		cerr << "keeperTileWidths: num takers must be at least 2, found: " 
			<< numT << endl;
		return 0;
	}

	int j = 0;

	//////shiva start
	for(int i = 1; i < numK; i++)//KiK distance
	{
		ranges[j] = 100.0;
		minValues[j] = 0;
		resolutions[j++] = 3.0;
	}

	for(int i = 1; i < numK; i++)//KiMinDisT
	{
		ranges[j] = 100.0;
		minValues[j] = 0;
		resolutions[j++] = 3.0;
	}

	for(int i = 1; i < numK; i++)//KiMinAngCloseT
	{
		ranges[j] = 180.0;
		minValues[j] = 0;
		resolutions[j++] = 10.0;
	}

	//K1MinDisTInCone
	ranges[j] = 100.0;
	minValues[j] = 0;
	resolutions[j++] = 3.0;

	//K1MinDisT
	ranges[j] = 100.0;
	minValues[j] = 0;
	resolutions[j++] = 3.0;


	for(int i = 0; i < numK; i++)//KDisGoal
	{
		ranges[j] = 100.0;
		minValues[j] = 0;
		resolutions[j++] = 3.0;
	}

	//K1MaxAngGoal
	ranges[j] = 180.0;
	minValues[j] = 0;
	resolutions[j++] = 10.0;

	//K1DisGoalie
	ranges[j] = 100.0;
	minValues[j] = 0;
	resolutions[j++] = 1.0;

	return j;
}

/******************************************************************************************************/

int WorldModel::SGkeeperStateRangesAndResolutionsMin( double ranges[], 
		double minValues[], 
		double resolutions[], 
		int numK, int numT )
{
	if ( numK < 3 ) {
		cerr << "keeperTileWidths: num keepers must be at least 3, found: " 
			<< numK << endl;
		return 0;
	}

	if ( numT < 2 ) {
		cerr << "keeperTileWidths: num takers must be at least 2, found: " 
			<< numT << endl;
		return 0;
	}

	int j = 0;

	//////shiva start
	for(int i = 1; i < numK; i++)//KiK distance
	{
		ranges[j] = 100.0;
		minValues[j] = 0;
		resolutions[j++] = 3.0;
	}

	for(int i = 1; i < numK; i++)//KiMinDisT
	{
		ranges[j] = 100.0;
		minValues[j] = 0;
		resolutions[j++] = 3.0;
	}

	for(int i = 1; i < numK; i++)//KiMinAngCloseT
	{
		ranges[j] = 180.0;
		minValues[j] = 0;
		resolutions[j++] = 10.0;
	}

	//K1MinDisTInCone
	ranges[j] = 100.0;
	minValues[j] = 0;
	resolutions[j++] = 3.0;

	//K1MinDisT
	ranges[j] = 100.0;
	minValues[j] = 0;
	resolutions[j++] = 3.0;


	for(int i = 0; i < numK; i++)//KDisGoal
	{
		ranges[j] = 100.0;
		minValues[j] = 0;
		resolutions[j++] = 3.0;
	}

	//K1MaxAngGoal
	ranges[j] = 180.0;
	minValues[j] = 0;
	resolutions[j++] = 10.0;

	//K1DisGoalie
	ranges[j] = 100.0;
	minValues[j] = 0;
	resolutions[j++] = 1.0;

	return j;
}
/******************************************************************************************************/

int WorldModel::KWYkeeperStateVars( double state[] )
{
	ObjectT K1 = getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL );
	/*
	   if ( !SoccerTypes::isTeammate( K1 ) )
	   return 0; // maybe change to != agentObject
	 */

	int numK = getNumKeepers();
	int numT = getNumTakers();

	VecPosition posK1 = getGlobalPosition(K1);

	ObjectT K[ numK ];
	for ( int i = 0; i < numK; i++ )
		K[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );

	ObjectT T[ numT ];
	for ( int i = 0; i < numT; i++ )
		T[ i ] = SoccerTypes::getOpponentObjectFromIndex( i );

	double WB_dist_to_K[ numK ];
	if ( !sortClosestTo( K, numK, K1, WB_dist_to_K ) )
		return 0;

	double WB_dist_to_T[ numT ];
	if ( !sortClosestTo( T, numT, K1, WB_dist_to_T ) )
		return 0;

	///////////////////////////////////////////////////////////////////////////////////////


	int j = 0;
        //cout << "numK " << numK << endl;
	for(int i = 1; i < numK; i++){
		state[j++] = posK1.getDistanceTo(getGlobalPosition(K[i]));
		//cout << "Astatecheck " << j-1 << " dist" << endl;
	}
	for(int i = 1; i < numK; i++){
		state[j++] = minDisToT(getGlobalPosition(K[i]));
		//cout << "Bstatecheck " << j-1 << " dist" << endl;
	}
	for(int i = 1; i < numK; i++){
		state[j++] = minAngWithCloseT(posK1, getGlobalPosition(K[i]));
		//cout << "Cstatecheck " << j-1 << " ang" << endl;
	}
	state[j++] = KWYminDisToTInCone(posK1);	
	//cout << "Dstatecheck " << j-1 << " dist" << endl;
	state[j++] = minDisToT(posK1);	
	//cout << "Estatecheck " << j-1 << " dist" << endl;
	/*
	   fstream file;
	   file.open("./alpha.txt", ios::out | ios::app);
	   file<<endl<<"Inside WMSTATEVARS"<<endl;
	   file<<"Cycle: " << getCurrentCycle();
	   for(int i = 0; i < j; i++)
	   file << i << "\t" << state[i] << "\n";
	   file <<	"K: " << getNumKeepers() << "\tT: " << getNumTakers();
	   pthread_t tid;
	   tid = pthread_self();
	   file << endl << tid << "***********************************************" << "\n";	

	   file.close();	
	 */
	return j;
}

// Greg:
// This really doesn't belong here,
// because it is related to a specific 
// learner.  I don't know exactly
// where to put it, though because
// I want to keep the LinearSarsa
// class generic.

// Yaxin: changed from keeperTileWidths to keeperResolutions and keeperRanges,

int WorldModel::KWYkeeperStateRangesAndResolutions( double ranges[], 
		double minValues[], 
		double resolutions[], 
		int numK, int numT )
{
	if ( numK < 3 ) {
		cerr << "keeperTileWidths: num keepers must be at least 3, found: " 
			<< numK << endl;
		return 0;
	}

	if ( numT < 2 ) {
		cerr << "keeperTileWidths: num takers must be at least 2, found: " 
			<< numT << endl;
		return 0;
	}

	int j = 0;

	//////shiva start
	for(int i = 1; i < numK; i++)//KiK distance
	{
		ranges[j] = 100.0;
		minValues[j] = 0;
		resolutions[j++] = 3.0;
	}

	for(int i = 1; i < numK; i++)//KiMinDisT
	{
		ranges[j] = 100.0;
		minValues[j] = 0;
		resolutions[j++] = 3.0;
	}

	for(int i = 1; i < numK; i++)//KiMinAngCloseT
	{
		ranges[j] = 180.0;
		minValues[j] = 0;
		resolutions[j++] = 10.0;
	}

	//K1MinDisTInCone
	ranges[j] = 100.0;
	minValues[j] = 0;
	resolutions[j++] = 3.0;

	//K1MinDisT
	ranges[j] = 100.0;
	minValues[j] = 0;
	resolutions[j++] = 3.0;

	return j;
}

void WorldModel::setNewTrainerMessageHeard()
{
	m_newTrainerMessageHeard = true;
}
void WorldModel::resetNewTrainerMessageHeard()
{
	m_newTrainerMessageHeard = false;
}
bool WorldModel::isNewTrainerMessageHeard()
{
	return m_newTrainerMessageHeard;
}

int WorldModel::timeLastTrainerMessageHeard()
{
	return m_timeLastTrainerMessageHeard;
}

void WorldModel::setTimeLastTrainerMessageHeard(int time)
{
	m_timeLastTrainerMessageHeard = time;
}

int WorldModel::timeLastSayMessageSent()
{
	return m_timeLastSayMessageSent;
}

void WorldModel::setTimeLastSayMessageSent(int time)
{
	m_timeLastSayMessageSent = time;
}

void WorldModel::setTaskType(int t)
{
	taskType = t;
}

int WorldModel::getTaskType()
{
	return taskType;
} 

void WorldModel::setGameCondition(int c)
{
	gameCondition = c;
}

int WorldModel::getGameCondition()  
{
	return gameCondition;
}

void WorldModel::setMoveSpeed( double speed )
{
	m_moveSpeed = speed;
}

double WorldModel::getMoveSpeed()
{
	return m_moveSpeed;
}

void WorldModel::setKeepawayRect( VecPosition pos1, VecPosition pos2 )
{
	m_keepawayRect.setRectanglePoints( pos1, pos2 );
}

Rect WorldModel::getKeepawayRect()
{
	return m_keepawayRect;
}

double WorldModel::minDisToT(VecPosition point)
{
	double minDis;
	int numT;
	int i;
	double tempDis;

	numT = getNumTakers();
	ObjectT T[numT];

	if(getSide() == SIDE_LEFT)//Keeper
		for(i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getOpponentObjectFromIndex(i);
	else//Taker
		for(i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getTeammateObjectFromIndex(i);

	minDis = 1000.0;//infinity
	for(i = 0; i < numT; i++)
	{
		tempDis = point.getDistanceTo(getGlobalPosition(T[i]));

		if(minDis > tempDis)
			minDis = tempDis;		
	}	

	return minDis;
}

double WorldModel::minAngWithT(VecPosition point1, VecPosition point2)//returns the minimum T[i]-point1-point2 value 
{
	double minAng;

	int numT = getNumTakers();
	ObjectT T[numT];

	if(getSide() == SIDE_LEFT)//Keeper
		for(int i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getOpponentObjectFromIndex(i);
	else//Taker	
		for(int i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getTeammateObjectFromIndex(i);

	minAng = 360.0;//infinity
	for(int i = 0; i < numT; i++)
	{
		double tempAng = point1.getAngleBetweenPoints(point2, getGlobalPosition(T[i]));

		if(minAng > tempAng)
			minAng = tempAng;		
	}	

	return minAng;
}

double WorldModel::minAngWithTInCircle(VecPosition point1, VecPosition point2, double radius)//returns the minimum T[i]-point1-point2 value 
{//T[i] is overlooked if it is outside the circle with point1 as centre, radius as the radius.
	double minAng;

	int numT = getNumTakers();
	ObjectT T[numT];

	if(getSide() == SIDE_LEFT)//Keeper
		for(int i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getOpponentObjectFromIndex(i);
	else//Taker	
		for(int i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getTeammateObjectFromIndex(i);

	///double radius = (point2 - point1).getMagnitude();

	minAng = 360.0;//infinity
	for(int i = 0; i < numT; i++)
	{
		if(point1.getDistanceTo(getGlobalPosition(T[i])) > radius)
			continue;

		double tempAng = point1.getAngleBetweenPoints(point2, getGlobalPosition(T[i]));

		if(minAng > tempAng)
			minAng = tempAng;		
	}	

	return minAng;
}

double WorldModel::minAngWithCloseT(VecPosition point1, VecPosition point2)//returns the minimum T[i]-point1-point2 value 
{//T[i] is overlooked if it is outside the circle with point1 as centre, radius as the radius.
	double minAng;

	int numT = getNumTakers();
	ObjectT T[numT];

	if(getSide() == SIDE_LEFT)//Keeper
		for(int i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getOpponentObjectFromIndex(i);
	else//Taker	
		for(int i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getTeammateObjectFromIndex(i);

	minAng = 360.0;//infinity
	for(int i = 0; i < numT; i++)
	{
		VecPosition TPos = getGlobalPosition(T[i]);
		if((point1.getDistanceTo(TPos) + point2.getDistanceTo(TPos)) > (2.0 * point1.getDistanceTo(point2)))
			continue;

		double tempAng = point1.getAngleBetweenPoints(point2, TPos);

		if(minAng > tempAng)
			minAng = tempAng;		
	}	

	return minAng;
}


double WorldModel::SGminDisToTInCone(VecPosition point)
{
	double minDis;
	int numT;
	int i;
	double TDis, TAng;
	VecPosition goalCentre;

	numT = getNumTakers();
	ObjectT T[numT];

	if(getSide() == SIDE_LEFT)//Keeper  	
	{
		for(i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getOpponentObjectFromIndex(i);

		goalCentre = VecPosition(PITCH_LENGTH / 2.0, 0);
	}
	else//Taker  	
	{
		for(i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getTeammateObjectFromIndex(i);

		goalCentre = VecPosition(-PITCH_LENGTH / 2.0, 0);
	}


	minDis = getConeRadius();

	for(i = 0; i < numT; i++)
	{
		VecPosition TPos = getGlobalPosition(T[i]);

		TDis = point.getDistanceTo(TPos);
		TAng = point.getAngleBetweenPoints(goalCentre, TPos);

		if(TDis <= getConeRadius())
			if(TAng <= getConeAngle())//T[i] is inside cone
			{
				if(minDis > TDis)
					minDis = TDis;		
			} 
	}	

	return minDis;
}

double WorldModel::KWYminDisToTInCone(VecPosition point)
{
	double minDis;
	int numT;
	int i;
	double TDis, TAng;
	VecPosition centre;

	numT = getNumTakers();
	ObjectT T[numT];

	if(getSide() == SIDE_LEFT)//Keeper  	
	{
		for(i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getOpponentObjectFromIndex(i);

		centre = VecPosition(0, 0);
	}
	else//Taker  	
	{
		for(i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getTeammateObjectFromIndex(i);

		centre = VecPosition(0, 0);
	}


	minDis = getConeRadius();

	for(i = 0; i < numT; i++)
	{
		VecPosition TPos = getGlobalPosition(T[i]);

		TDis = point.getDistanceTo(TPos);
		TAng = point.getAngleBetweenPoints(centre, TPos);

		if(TDis <= getConeRadius())
			if(TAng <= getConeAngle())//T[i] is inside cone
			{
				if(minDis > TDis)
					minDis = TDis;		
			} 
	}	

	return minDis;
}

double WorldModel::getConeAngle()
{
	return 60.0;//Degrees
}

double WorldModel::getConeRadius()
{
	return 40.0;//Metres

}

double WorldModel::distanceToGoal(VecPosition point)
{
	VecPosition goalRightUpright, goalLeftUpright;

	if(getSide() == SIDE_LEFT)//Keeper
	{
		goalRightUpright = VecPosition(PITCH_LENGTH / 2.0, SS->getGoalWidth() / 2.0);
		goalLeftUpright = VecPosition(PITCH_LENGTH / 2.0, -SS->getGoalWidth() / 2.0);
	}
	else//Taker
	{
		goalRightUpright = VecPosition(-PITCH_LENGTH / 2.0, SS->getGoalWidth() / 2.0);
		goalLeftUpright = VecPosition(-PITCH_LENGTH / 2.0, -SS->getGoalWidth() / 2.0);
	}

	if(point.getY() < goalLeftUpright.getY())
		return point.getDistanceTo(goalLeftUpright);
	if(point.getY() > goalRightUpright.getY())
		return point.getDistanceTo(goalRightUpright);

	return fabs(goalLeftUpright.getX() - point.getX());		
}

double WorldModel::distanceToGoalie(VecPosition point)
{
	VecPosition goaliePos;

	int numT = getNumTakers();

	if(getSide() == SIDE_LEFT)//Keeper
		goaliePos = getGlobalPosition(SoccerTypes::getOpponentObjectFromIndex(numT - 1));
	else//Taker
		goaliePos = getGlobalPosition(SoccerTypes::getTeammateObjectFromIndex(numT - 1));

	double distance = point.getDistanceTo(goaliePos);

	return distance;		
}

double WorldModel::maxAngWithGoal(VecPosition point, VecPosition *bestPoint)
{
	VecPosition goalLeftUpright, goalRightUpright;

	int numT = getNumTakers();
	ObjectT T[numT];

	if(getSide() == SIDE_LEFT)//Keeper
	{
		for(int i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getOpponentObjectFromIndex(i);

		goalRightUpright = VecPosition(PITCH_LENGTH / 2.0, SS->getGoalWidth() / 2.0);
		goalLeftUpright = VecPosition(PITCH_LENGTH / 2.0, -SS->getGoalWidth() / 2.0);
	}
	else//Taker
	{
		for(int i = 0; i < numT; i++ )
			T[i] = SoccerTypes::getTeammateObjectFromIndex(i);

		goalRightUpright = VecPosition(-PITCH_LENGTH / 2.0, SS->getGoalWidth() / 2.0);
		goalLeftUpright = VecPosition(-PITCH_LENGTH / 2.0, -SS->getGoalWidth() / 2.0);
	}

	double V[numT + 1];
	int VCount;
	double TAngWithRightUpright, TAngWithLeftUpright;
	VecPosition TPos;
	double goalAngle;

	double maxAng, bestAngle;

	goalAngle = point.getAngleBetweenPoints(goalRightUpright, goalLeftUpright);

	VCount = 0;
	for(int i = 0; i < numT; i++)
	{
		TPos = getGlobalPosition(T[i]);

		TAngWithRightUpright = point.getAngleBetweenPoints(goalRightUpright, TPos);
		TAngWithLeftUpright = point.getAngleBetweenPoints(TPos, goalLeftUpright);

		if((TAngWithRightUpright < goalAngle) && (TAngWithLeftUpright < goalAngle))
		{
			V[VCount] = TAngWithRightUpright;
			VCount++;
		}
	}

	V[VCount] = goalAngle;
	VCount++;

	////////sort VCount
	for(int i = 0; i < VCount - 1; i++)
		for(int k = 0; k < VCount - i - 1; k++)
		{
			if(V[k] > V[k + 1])
			{
				double temp = V[k];
				V[k] = V[k + 1];
				V[k + 1] = temp;
			}		
		}	

	maxAng = V[0];
	bestAngle = V[0] * 0.2;

	for(int i = 1; i < VCount; i++)
	{
		if(maxAng < (V[i] - V[i - 1]))
		{
			maxAng = V[i] - V[i - 1];
			if(i == VCount - 1)
				bestAngle = (V[i] * 0.8) + (V[i - 1] * 0.2);
			else
				bestAngle = (V[i] * 0.5) + (V[i - 1] * 0.5);
		}	
	}		

	//I am not sure how the angles are defined: this just seems to work fine.
	if(getSide() == SIDE_LEFT)//Keeper
		bestAngle = ((goalRightUpright - point).rotate(-bestAngle)).getDirection();
	else//Taker
		bestAngle = ((goalRightUpright - point).rotate(bestAngle)).getDirection();

	Line bestLine = Line::makeLineFromPositionAndAngle(point, bestAngle);
	Line goalLine = Line::makeLineFromTwoPoints(goalRightUpright, goalLeftUpright);

	*bestPoint = goalLine.getIntersection(bestLine);

	return maxAng;
}

//Both pass by reference, so be careful while using.
void WorldModel::setReceivedState(double state[])
{
	copyState(state, m_receivedState);
}
void WorldModel::getReceivedState(double state[])
{
	copyState(m_receivedState, state);
}

void WorldModel::copyState(double sourceState[], double destinationState[])
{
	double dummyRanges[100];
	double dummyMinValues[100];
	double dummyResolutions[100];
	int dummyNumKeepers = getNumKeepers();
	int dummyNumTakers = getNumTakers();

	int numStates = SGkeeperStateRangesAndResolutions(dummyRanges, dummyMinValues, dummyResolutions, dummyNumKeepers, dummyNumTakers);

	for(int i = 0; i < numStates; i++)
		destinationState[i] = sourceState[i];
}


void WorldModel::setReceivedAction(int action)
{
	m_receivedAction = action;
}
int WorldModel::getReceivedAction()
{
	return m_receivedAction;
}

void WorldModel::shootgoalTaskAnalyseTrainerMessage(char *strMsg)
{
	/*
	   fstream file;
	   file.open("beta.txt", ios::out | ios :: app);
	   file << endl << "Cycle: " << getCurrentCycle() << " Player " << getPlayerNumber() << " Trainer Message(B): " << strMsg;
	   file.close();
	 */

	int i = 0;

	while((strMsg[i] != '*') && (strMsg[i] != '\0'))
		i++;

	if(strMsg[i] == '\0')
		return;//This ws a bogus message!	

	i++;

	if(strMsg[i] == 'r')
	{
		resetEpisode();
		setGameCondition(GC_RESET);
	}		
	else if(strMsg[i] == 'g')
	{
		resetEpisode();
		setGameCondition(GC_GOAL);
	}		
	else if(strMsg[i] == 'o')
	{
		resetEpisode();
		setGameCondition(GC_BALL_OUT_OF_PLAY);
	}		
	else if(strMsg[i] == 'c')
	{
		resetEpisode();
		setGameCondition(GC_CAUGHT_BY_GOALIE);
	}		
	else if(strMsg[i] == 't')
	{
		resetEpisode();

		int g = GC_INVALID;
		switch(strMsg[i + 1])
		{
			case '0': g = GC_WITH_TAKER_0; break;
			case '1': g = GC_WITH_TAKER_1; break;
			case '2': g = GC_WITH_TAKER_2; break;
			case '3': g = GC_WITH_TAKER_3; break;
			default: break;//invalid
		}

		setGameCondition(g);
	}		
	else if(strMsg[i] == 'k')
	{
		int g = GC_INVALID;
		switch(strMsg[i + 1])
		{
			case '0': g = GC_WITH_KEEPER_0; break;
			case '1': g = GC_WITH_KEEPER_1; break;
			case '2': g = GC_WITH_KEEPER_2; break;
			case '3': g = GC_WITH_KEEPER_3; break;
			default: break;//invalid
		}
		setGameCondition(g);
	}
	else
	{
		//Something fishy!
		return;
	}


	if(taskType != TASK_SHOOTGOALVOICED)//It must be TASK_KEEPAWAY or TASK_SHOOTGOAL or INVALID
	{
		setNewTrainerMessageHeard();
	}

	int j, k;
	double dummyRanges[100];
	double dummyMinValues[100];
	double dummyResolutions[100];
	int dummyNumKeepers = getNumKeepers();
	int dummyNumTakers = getNumTakers();

	int sizeD = sizeof(double);
	int sizeI = sizeof(int);

	double tempReceivedState[100];
	int tempReceivedAction;
	double tempReceivedReward;

	int tempInt;
	double tempDouble;

	char buffer[100];  

	int numStates = SGkeeperStateRangesAndResolutions(dummyRanges, dummyMinValues, dummyResolutions, dummyNumKeepers, dummyNumTakers);


	//fstream file;

	while((strMsg[i] != '*') && (strMsg[i] != '\0'))
		i++;

	if(strMsg[i] == '\0')
		return;//This was a bogus message!	

	i++;

	for(j = 0; j < numStates; j++)
	{
		k = 0;
		while(strMsg[i] != ' ')
		{
			if(strMsg[i] == '\0')
				return;

			buffer[k] = strMsg[i];
			i++;
			k++;
		}
		buffer[k] = ' ';
		i++;
		sscanf(buffer, "%lf", &tempDouble);
		tempReceivedState[j] = tempDouble; 
	}		

	i++;

	k = 0;
	while(strMsg[i] != ' ')
	{
		if(strMsg[i] == '\0')
			return;

		buffer[k] = strMsg[i];
		i++;
		k++;
	}
	buffer[k] = ' ';
	i++;

	sscanf(buffer, "%d", &tempInt);
	tempReceivedAction = tempInt;

	/*
	   while((strMsg[i] != '*') && (strMsg[i] != '\0'))
	   i++;
	   if(strMsg[i] == '\0')
	   return;//This was a bogus message!	

	   i++;

	   k = 0;
	   while(strMsg[i] != ' ')
	   {
	   if(strMsg[i] == '\0')
	   return;

	   buffer[k] = strMsg[i];
	   i++;
	   k++;
	   }
	   buffer[k] = ' ';
	   i++;
	   sscanf(buffer, "%lf", &tempDouble);
	   tempReceivedReward = tempDouble;

	//fstream file;
	if(getPlayerNumber() == 1)
	{/*

	file.open("alpha.txt", ios::out | ios :: app);
	file << endl << "Cycle: " << getCurrentCycle() << endl  << "My Number: " << getPlayerNumber() << endl;
	file << "Received: " << endl;
	for(i = 0; i < numStates; i++)
	file << endl << tempReceivedState[i];

	file << endl << tempReceivedAction << " ";
	file << endl << tempReceivedReward;
	pthread_t tid;
	tid = pthread_self();
	file << endl << tid << "***************FFFFFFFFFFFFFF********************************" << "\n";	
	file.close();
	}
	 */

	//Having reached here means that strMsg did not end prematurely

	setReceivedState(tempReceivedState);

	setReceivedAction(tempReceivedAction);

	//setReceivedReward(tempReceivedReward);

	setNewTrainerMessageHeard();

	//fstream file;
	/*file.open("beta.txt", ios::out | ios :: app);
	  file << endl << "Cycle: " << getCurrentCycle() << " Player " << getPlayerNumber() << " Trainer Message: " << strMsg;
	  file.close();
	 */


	return;
}

char*  WorldModel::shootgoalTaskMakeStringToSay(double state[], int action)
{
	int i, j, k;

	double dummyRanges[100];
	double dummyMinValues[100];
	double dummyResolutions[100];
	int dummyNumKeepers = getNumKeepers();
	int dummyNumTakers = getNumTakers();

	int sizeD = sizeof(double);
	int sizeI = sizeof(int);

	char strMsg[600];
	char buffer[100];  

	int numStates = SGkeeperStateRangesAndResolutions(dummyRanges, dummyMinValues, dummyResolutions, dummyNumKeepers, dummyNumTakers);

	i = 0;

	strMsg[i] = '*';
	i++;
	strMsg[i] = 'k';
	i++;
	strMsg[i] = getPlayerNumber() - 1 + '0';//This assumes my number is a single digit.
	i++;
	strMsg[i] = ' ';
	i++;

	strMsg[i] = '*';
	i++;

	for(j = 0; j < numStates; j++)
	{
		sprintf(buffer, "%lf #", state[j]);
		k = 0;
		while(buffer[k] != '#')
		{
			strMsg[i] = buffer[k];
			i++;
			k++;
		}	
	}

	strMsg[i] = '*';
	i++;

	sprintf(buffer, "%d #", action);
	k = 0;
	while(buffer[k] != '#')
	{
		strMsg[i] = buffer[k];
		i++;
		k++;
	}	

	/*	sprintf(buffer, "%lf #", reward);
		k = 0;
		while(buffer[k] != '#')
		{
		strMsg[i] = buffer[k];
		i++;
		k++;
		}	
	 */	
	strMsg[i] = '\0';
	/*
	   fstream file;
	   file.open("alpha.txt", ios::out | ios :: app);
	   file << endl << "Cycle: " << getCurrentCycle() << endl  << "My Number: " << getPlayerNumber() << endl;
	   file << "Sending: " << endl << strMsg;
	   file << "**********************" << endl; 
	   file.close();
	 */
	return strMsg;
}

