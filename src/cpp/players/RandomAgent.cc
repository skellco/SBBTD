#include <fcntl.h>
#include "RandomAgent.h"
#include "LoggerDraw.h"
#include <fstream>
#include <math.h>
#include <stdlib.h>

// If all is well, there should be no mention of anything keepaway- or soccer-
// related in this file. 

extern LoggerDraw LogDraw;

RandomAgent::RandomAgent( int numFeatures, int numActions, int id)
		:PolicyAgent(numFeatures, numActions, id)			
{
}

int RandomAgent::selectAction(double state[], double episodeTime)//This method make sspecific assumptions about the shootgoal task
{
	return (int)(drand48() * getNumActions());
}

int RandomAgent::bestAction(double state[])
{
  	return selectAction(state, -1);
}

