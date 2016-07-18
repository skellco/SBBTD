#include "soccer.h"

/***********************************************************************************************************/
point * initUniformPointHalfFieldOffense(long gtime, long dim, long label, int phase)
{
   double maxDist = sas((double)SOCCER_FIELD_SIZE,(double)SOCCER_FIELD_SIZE,90);
   vector < double > state;

   state.push_back(drand48()*maxDist); // 0
   state.push_back(drand48()*maxDist); // 1
   state.push_back(drand48()*maxDist); // 2
   state.push_back(drand48()*maxDist); // 2
   state.push_back(drand48()*maxDist); // 4
   state.push_back(drand48()*maxDist); // 5
   state.push_back(drand48()*SOCCER_MAX_ANGLE);  // 6
   state.push_back(drand48()*SOCCER_MAX_ANGLE);  // 7
   state.push_back(drand48()*SOCCER_MAX_ANGLE);  // 8
   state.push_back(drand48()*maxDist); // 9
   state.push_back(drand48()*maxDist); // 10
   state.push_back(drand48()*maxDist); // 11
   state.push_back(drand48()*maxDist); // 12
   state.push_back(drand48()*maxDist); // 13
   state.push_back(drand48()*maxDist); // 14
   state.push_back(drand48()*SOCCER_MAX_ANGLE);  // 15
   state.push_back(drand48()*maxDist); // 16

   return new point(label, dim, gtime, &state[0], label,-1.0,-1.0,-1.0,phase);
}

/***********************************************************************************************************/
point * initUniformPointHalfFieldKeepaway(long gtime, long dim, long label, int phase)
{
   double maxDist = sas((double)SOCCER_FIELD_SIZE,(double)SOCCER_FIELD_SIZE,90);
   vector < double > state;

   state.push_back(drand48()*maxDist); // 0
   state.push_back(drand48()*maxDist); // 1
   state.push_back(drand48()*maxDist); // 2
   state.push_back(drand48()*maxDist); // 2
   state.push_back(drand48()*maxDist); // 4
   state.push_back(drand48()*maxDist); // 5
   state.push_back(drand48()*SOCCER_MAX_ANGLE);  // 6
   state.push_back(drand48()*SOCCER_MAX_ANGLE);  // 7
   state.push_back(drand48()*SOCCER_MAX_ANGLE);  // 8
   state.push_back(drand48()*maxDist); // 9
   state.push_back(drand48()*maxDist); // 10

   return new point(label, dim, gtime, &state[0], label,-1.0,-1.0,-1.0,phase);
}

/***********************************************************************************************************/
void keepawayDiscretizeState(vector < double > &state, int steps)
{
   double maxDistToC = sas(SOCCER_FIELD_SIZE/2,SOCCER_FIELD_SIZE/2,90);
   double maxDist = sas(SOCCER_FIELD_SIZE,SOCCER_FIELD_SIZE,90);
   state[0] = discretize(state[0],0.0,maxDist,steps);
   state[1] = discretize(state[1],0.0,maxDist,steps);
   state[2] = discretize(state[2],0.0,maxDist,steps);
   state[3] = discretize(state[3],0.0,maxDist,steps);
   state[4] = discretize(state[4],0.0,maxDist,steps);
   state[5] = discretize(state[5],0.0,maxDist,steps);
   state[6] = discretize(state[6],0.0,SOCCER_MAX_ANGLE,steps);
   state[7] = discretize(state[7],0.0,SOCCER_MAX_ANGLE,steps);
   state[8] = discretize(state[8],0.0,SOCCER_MAX_ANGLE,steps);
   state[9] = discretize(state[9],0.0,maxDist,steps);
   state[10] = discretize(state[10],0.0,maxDist,steps);
}

/***********************************************************************************************************/
void halfFieldDiscretizeState(vector < double > &state, int steps)
{
   double maxDistToC = sas(SOCCER_FIELD_SIZE/2,SOCCER_FIELD_SIZE/2,90);
   double maxDist = sas(SOCCER_FIELD_SIZE,SOCCER_FIELD_SIZE,90);
   state[0] = discretize(state[0],0.0,maxDist,steps);
   state[1] = discretize(state[1],0.0,maxDist,steps);
   state[2] = discretize(state[2],0.0,maxDist,steps);
   state[3] = discretize(state[3],0.0,maxDist,steps);
   state[4] = discretize(state[4],0.0,maxDist,steps);
   state[5] = discretize(state[5],0.0,maxDist,steps);
   state[6] = discretize(state[6],0.0,SOCCER_MAX_ANGLE,steps);
   state[7] = discretize(state[7],0.0,SOCCER_MAX_ANGLE,steps);
   state[8] = discretize(state[8],0.0,SOCCER_MAX_ANGLE,steps);
   state[9] = discretize(state[9],0.0,maxDist,steps);
   state[10] = discretize(state[10],0.0,maxDist,steps);
   state[11] = discretize(state[11],0.0,maxDist,steps);
   state[12] = discretize(state[12],0.0,maxDist,steps);
   state[13] = discretize(state[13],0.0,maxDist,steps);
   state[14] = discretize(state[14],0.0,maxDist,steps);
   state[15] = discretize(state[15],0.0,SOCCER_MAX_ANGLE,steps);
   state[16] = discretize(state[16],0.0,maxDist,steps);
}

void mapKeepawayToHalfFieldActions(set <learner *> &l){
   set < learner * > :: iterator leiter, leiterEnd;
   for(leiter = l.begin(); leiter != l.end(); leiter++)
      (*leiter)->action((*leiter)->action()+1);
}

