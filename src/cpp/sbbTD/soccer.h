#ifndef SOCCER_H
#define SOCCER_H
#include "sbbLearner.h"
#include "sbbMisc.h"
#include "sbbPoint.h"
#include "sbbTeam.h"

//#define SOCCER_FIELD_SIZE 60.0
//#define SOCCER_MAX_ANGLE 180
//#define TRAIN_REWARD 1
//#define TEST_REWARD 2
//#define REWARD_ONE 1
//#define REWARD_TWO 2

using namespace std;

point * initUniformPointHalfFieldOffense(long, long, long, int);
point * initUniformPointHalfFieldKeepaway(long, long, long, int);
void keepawayDiscretizeState(vector < double > &, int);
void halfFieldDiscretizeState(vector < double > &, int);
void mapKeepawayToHalfFieldActions(set <learner *> &);
#endif
