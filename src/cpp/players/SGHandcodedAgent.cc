#include <fcntl.h>
#include "ShootgoalPlayer.h"
#include "SGHandcodedAgent.h"
#include "LoggerDraw.h"
#include <fstream>
#include <math.h>

// If all is well, there should be no mention of anything keepaway- or soccer-
// related in this file. 

extern LoggerDraw LogDraw;

SGHandcodedAgent::SGHandcodedAgent( int numFeatures, int numActions, int id)
		:PolicyAgent(numFeatures, numActions, id)			
{
}

SoccerCommand SGHandcodedAgent::player(int & episodeCount)
{
  SoccerCommand soc;

  return soc;
}

int SGHandcodedAgent::selectAction(double state[], double episodeTime)//This method makes specific assumptions about the shootgoal task
{
  //cout << "Hand coded here!";
  //for (int i = 0; i < 17; i++)
  //    cout << " " << state[i];
  //cout <<  endl;
  int numK = 4;
  int numT = 5;
  
  int j = 0;
  
  double disK12 = state[j++];
  double disK13 = state[j++];
  double disK14 = state[j++];
 
  double K2MinDisT = state[j++];
  double K3MinDisT = state[j++];
  double K4MinDisT = state[j++];

  double K2MinAngCloseT = state[j++];
  double K3MinAngCloseT = state[j++];
  double K4MinAngCloseT = state[j++];
    
  double K1MinDisTInCone = state[j++];
 
  double K1MinDisT = state[j++];

  double K1DisGoal = state[j++];
  double K2DisGoal = state[j++];
  double K3DisGoal = state[j++];
  double K4DisGoal = state[j++];
  
  double K1MaxAngGoal = state[j++];

  double K1DisGoalie = state[j++];
  
  double pv2 = passValue(disK12, K2MinDisT, K2MinAngCloseT, K2DisGoal);
  double pv3 = passValue(disK13, K3MinDisT, K3MinAngCloseT, K3DisGoal);
  double pv4 = passValue(disK14, K4MinDisT, K4MinAngCloseT, K4DisGoal);
  
  int action;
  
  if((K1DisGoal < 15.0) && (K1MaxAngGoal > 20.0))
  {
  	action = ACTION_SG_SHOOT_GOAL;
  }
  else if((K1DisGoal < 25.0) && (K1MaxAngGoal > 40.0))
  {
  	action = ACTION_SG_SHOOT_GOAL;
  }
  else if((K1MinDisT > 6.0) && (K1MinDisTInCone > 10.0))
  {
  	action = ACTION_SG_DRIBBLE_NORMAL;
	//action = ACTION_SG_DRIBBLE_TO_GOAL;
  }
  else if((pv2 > pv3) && (pv2 > pv4))
  {
  	action = ACTION_SG_PASS_TO_K_2;
  }
  else if(pv3 > pv4)
  {
  	action = ACTION_SG_PASS_TO_K_3;
  }
  else
  {
  	action = ACTION_SG_PASS_TO_K_4;
  }
  
  return action;
}

double SGHandcodedAgent::passValue(double dis, double minDisT, double minAngT, double disGoal)
{

	double value;
	bool canPass;
	
	if(dis > 30.0)
		canPass = false;
	else if(((minDisT < 6.0) || (minAngT < 20.0)) && (disGoal > 20.0))
		canPass = false;
	else if(((minDisT < 4.0) || (minAngT < 15.0)) && (disGoal <= 20.0))
		canPass = false;
	else
		canPass = true;
		
	if(canPass)				
	{
		value = 1000.0 - disGoal;
		return value;
	}
	
	value = minAngT;
	return value;
}


int SGHandcodedAgent::bestAction(double state[])
{
  	return selectAction(state,-1);
}

