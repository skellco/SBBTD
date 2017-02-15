#include <iomanip>      // std::setprecision
#include <iostream>
#include <atomic>
#ifndef WIN32
#include <unistd.h>
#endif
#include <lo/lo.h>
#include <lo/lo_cpp.h>
#include "../sbbTD/sbbTD.h"
#include <vector>
#include <ctime>
#include "Parse.h"
#define P_ADD_PROFILE_POINT 0.0005
#define MAX_BEHAVIOUR_STEPS 5
#define NUM_SENSOR_INPUTS 95
#define SBB_DIM 29
#define ATOMIC_ACCEPT 1
#define DOUBLE_DYNAMIC_RANGE 50.0
#define BOOL_DYNAMIC_RANGE 10.0

//int ppAct = 0;
//int pathToNearestPowerPill[] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0};
/***********************************************************************************************************************/
// Globals used by state_handler and runEval
vector < double > currentState;
vector < int > neighbours;
double gameScore;
double pillsEaten;
double ghostScore;
double xCoord;
double yCoord;
atomic<int> episodeEnd; //used for synching with background threads, so make atomic
atomic<bool> newState; //used for synching with background threads, so make atomic

/*
   SENSOR INPUTS
   We have 95 sensor inputs: 7 non-directed  + 22 directed for each of 4 directions.

Note: B is for Boolean (0 or 1) | R is for Range (a number between and including 0 and 1)

For inputs [0 to 6]: [R, R, R, R, B, B, B]
For inputs [7 to 28]: [R, R, R, R, B, B, B, R, B, B, B, R, B, B, B, R, B, B, B, R, R, R]
For inputs [29 to 50]: Same as previous list of 22
For Inputs [51 to 72]: Same as previous list of 22
For inputs [73 to 94]: Same as previous list of 22

int[] B={0r,1r,2r,3r,4,5,6,
7r,8r,9r,10r,11,12,13,14r,15,16,17,18r,19,20,21,22r,23,24,25,26r,27r,28r,
29r,30r,31r,32r,33,34,35,36r,37,38,39,40r,41,42,43,44r,45,46,47,48r,49r,50r,
51r,52r,53r,54r,55,56,57,58r,59,60,61,62r,63,64,65,66r,67,68,69,70r,71r,72r,
73r,74r,75r,76r,77,78,79,80r,81,82,83,84r,85,86,87,88r,89,90,91,92r,94r,94r
*/
// Global used by mspacmanDiscretizeState and scaleState
vector<int> BOOL_INPUTS = {4,5,6,
   11,12,13,15,16,17,19,20,21,23,24,25,
   33,34,35,37,38,39,41,42,43,45,46,47,
   55,56,57,59,60,61,63,64,65,67,68,69,
   77,78,79,81,82,83,85,86,87,89,90,91};
bool stateVarIsBool[NUM_SENSOR_INPUTS];
/***********************************************************************************************************************/
void mspacmanDiscretizeState(vector < double > &state, int steps)
{
   for (int i = 0; i < state.size(); i++) {
      //if (find(BOOL_INPUTS.begin(), BOOL_INPUTS.end(),i) != BOOL_INPUTS.end())
      if (stateVarIsBool[i])
         state[i] = discretize(state[i], -(BOOL_DYNAMIC_RANGE/2), BOOL_DYNAMIC_RANGE/2,steps);
      else
         state[i] = discretize(state[i],0, DOUBLE_DYNAMIC_RANGE, steps);
   }
}

/***********************************************************************************************************************/
int end_handler(const char *path, const char *types, lo_arg ** argv,
      int argc, void *data, void *user_data)
{
   episodeEnd.store(1.0);
}
/***********************************************************************************************************************/
int state_handler(const char *path, const char *types, lo_arg ** argv,
      int argc, void *data, void *user_data)
{
   //episodeEnd.store((int)argv[0]->f);
   gameScore = argv[1]->f;
   pillsEaten = argv[2]->f;
   ghostScore = argv[3]->f;
   xCoord = argv[4]->f;
   yCoord = argv[5]->f;
   neighbours[0] = argv[6]->f;
   neighbours[1] = argv[7]->f;
   neighbours[2] = argv[8]->f;
   neighbours[3] = argv[9]->f;

   for (int i = 0; i < NUM_SENSOR_INPUTS; i++) {
      currentState[i] = argv[i+10]->f;
   }

   // cout << " episodeEnd " << episodeEnd.load() << " gameScore " << gameScore << " pillsEaten " << pillsEaten << " ghostScore " << ghostScore;
   // cout << " xCoord " << xCoord << " yCoord " << yCoord;
   // cout << " neighbours " << neighbours[0] << " " << neighbours[1] << " " << neighbours[2] << " " <<neighbours[3];
   // cout << " state:";
   // 
   // for (int i = 0; i < NUM_SENSOR_INPUTS; i++)
   //   cout << std::setprecision(5) << " " << currentState[i];
   // cout << endl;

   newState.store(true); 
   return 0;
}
/***********************************************************************************************************************/
void scaleState(vector < double > &state)
{
   for (int i = 0; i < state.size(); i++) {
      //if (find(BOOL_INPUTS.begin(), BOOL_INPUTS.end(),i) != BOOL_INPUTS.end())
      if (stateVarIsBool[i])
         state[i] = state[i]>0?BOOL_DYNAMIC_RANGE/2:-(BOOL_DYNAMIC_RANGE/2);
      else
         state[i] = state[i] * DOUBLE_DYNAMIC_RANGE;
   }
}
/***********************************************************************************************************************/
// dir must be one of 4 directions: 0-3. There are 29 inputs in total, 7 non-directed and 22 directed
void getDirectedState(vector < double > &currentState, vector < double > &directionalState, int dir){
   directionalState.clear();
   for (int i = 0; i < 7; i++)//7 non-directed inputs: 0-6
      directionalState.push_back(currentState[i]);
   for (int i = 0; i < 22; i++)//22 directed inputs; dir=0(7-28); dir=1(29-50); dir=2(51-72); dir=3(73-94)
      directionalState.push_back(currentState[(7+(dir*22))+i]);
}

/***********************************************************************************************************************/
// Checks if the nextAction will lead pacman into a wall
int isTowardWall(vector < int > &n, int nextAction) {
   if (n[nextAction] >= 0)
      return 0;
   else
      return 1;
} 
/***********************************************************************************************************************/
void init (vector <double> &s, vector < int > &n, int dim){
   s.clear();
   n.clear();

   for (int i = 0; i < dim; i++) {
      s.push_back(0);

      if (find(BOOL_INPUTS.begin(), BOOL_INPUTS.end(),i) != BOOL_INPUTS.end())
         stateVarIsBool[i] = true;
      else
         stateVarIsBool[i] = false;
   }

   for (int i = 0; i < 4; i++)
      n.push_back(0);
}
/***********************************************************************************************************************/
void runEval(lo::Address &e_policyAnimator, lo::Address &e_mspacmanServer, sbbTD &sbbEval, int e_t, int e_level, int e_phase, bool visual, int &timeGenTotalInGame, int hostToReplay){

   int timeStartGame;
   vector < double > behaviourSequence; // store a discretized trajectory for diversity maintenance
   vector < double > tmpBehaviourSequence;
   vector < double > directedState;
   vector < double > selectedDirectedState;
   int currentAction;
   int prevAction;
   int atomicAction;
   long step;
   long prevF;
   long decisionInstructions;
   long decisionInstructionsSum;
   vector <long> policyTreeTraceIds;
   int prevProfileId = -1;
   int newProfilePoints = 0;
   map<int,int> directedActions;
   map<double,int, std::greater<double>> acceptedDirectionPreferences;
   map<double,int, std::greater<double>> rejectedDirectionPreferences;
   map<double,int, std::greater<double>> acceptedDirectionPreferences_ID_1;
   map<double,int, std::greater<double>> rejectedDirectionPreferences_ID_1;
   map<double,int, std::greater<double>> acceptedDirectionPreferences_ID_0;
   map<double,int, std::greater<double>> rejectedDirectionPreferences_ID_0;
   long winningSymbiont_ID_1;
   long winningSymbiont_ID_0;
   long winningSymbiont_ID_1_prev;
   long winningSymbiont_ID_0_prev;
   vector < set <learner*, LearnerBidLexicalCompare> > learnersRanked(sbbEval.numLevels(),set < learner*,LearnerBidLexicalCompare>());
   episodeEnd.store(0);

   if (visual)
     e_mspacmanServer.send("delay","i",30);

   sbbEval.makeEvaluationVector(e_t,e_phase,false);
   if (sbbEval.evaluationVectorSize() > 0){
      int numEval = 0;
      if (hostToReplay >= 0){ 
         sbbEval.activeTeam(hostToReplay); 
         numEval = 3;
      }
      else {
         sbbEval.getFirstTeam();
         numEval = sbbEval.evaluationVectorSize();
      }

      if (visual){ e_mspacmanServer.send("visual"); numEval = 1; }


      for (int i = 0; i < numEval; i++){ 
         cout << "runEval " << i << " t " << e_t << " level " << e_level << " tm " << sbbEval.activeTeamId();
         e_mspacmanServer.send("start");
         prevAction = 0;
         prevF = 0;
         step = 0;
         decisionInstructionsSum = 0;
         timeStartGame = time(NULL);
         while (newState.load() == false && episodeEnd.load() < 1){ usleep(1000); } newState.store(false); //wait for initial state from server
         while (episodeEnd.load() < 1){ 
            //scaleState(currentState);
            acceptedDirectionPreferences.clear();
            rejectedDirectionPreferences.clear();
            acceptedDirectionPreferences_ID_1.clear();
            rejectedDirectionPreferences_ID_1.clear();
            acceptedDirectionPreferences_ID_0.clear();
            rejectedDirectionPreferences_ID_0.clear();
            for (int d = 0; d < 4; d++){ //0:UP 1:RIGHT 2:DOWN 3:LEFT
               if (!isTowardWall(neighbours,d)){
                  getDirectedState(currentState,directedState,d);
                  for (int l = 0; l < sbbEval.numLevels(); l++)learnersRanked[l].clear();
                  decisionInstructions = 0;
                  policyTreeTraceIds.clear();
                  atomicAction = sbbEval.getAction(directedState, e_phase==TRAIN_PHASE?true:false, learnersRanked,decisionInstructions, policyTreeTraceIds);
                  if (atomicAction == ATOMIC_ACCEPT){
                     acceptedDirectionPreferences.insert(pair<double,int>((*(learnersRanked[e_level].begin()))->key(),d));
                     acceptedDirectionPreferences_ID_1.insert(pair<double,int>((*(learnersRanked[e_level].begin()))->key(),(*(learnersRanked[e_level].begin()))->id()));
                     acceptedDirectionPreferences_ID_0.insert(pair<double,int>((*(learnersRanked[e_level].begin()))->key(),(*(learnersRanked[0].begin()))->id()));
                  }
                  else{
                     rejectedDirectionPreferences.insert(pair<double,int>((*(learnersRanked[e_level].begin()))->key(),d));
                     rejectedDirectionPreferences_ID_1.insert(pair<double,int>((*(learnersRanked[e_level].begin()))->key(),(*(learnersRanked[e_level].begin()))->id()));
                     rejectedDirectionPreferences_ID_0.insert(pair<double,int>((*(learnersRanked[e_level].begin()))->key(),(*(learnersRanked[0].begin()))->id()));
                  }
                  if (drand48() < P_ADD_PROFILE_POINT && sbbEval.activeTeamId() != prevProfileId){
                     sbbEval.addProfilePoint(directedState,sbbEval.dim(),gameScore,pillsEaten,ghostScore,e_phase,e_t);
                     prevProfileId = sbbEval.activeTeamId();
                     newProfilePoints++;
                  }
               }
            }
            winningSymbiont_ID_1_prev = winningSymbiont_ID_1;
            winningSymbiont_ID_0_prev = winningSymbiont_ID_0;
            if (acceptedDirectionPreferences.size() > 0){
               currentAction = acceptedDirectionPreferences.begin()->second;
               winningSymbiont_ID_1 = acceptedDirectionPreferences_ID_1.begin()->second;
               winningSymbiont_ID_0 = acceptedDirectionPreferences_ID_0.begin()->second;
            }
            else{
               currentAction = rejectedDirectionPreferences.rbegin()->second;
               winningSymbiont_ID_1 = rejectedDirectionPreferences_ID_1.rbegin()->second;//lowest bid (least rejected)
               winningSymbiont_ID_0 = rejectedDirectionPreferences_ID_0.begin()->second;//highest bid
            }
#ifdef PACMANDEBUG
            cout << "foundAct " << currentAction << " since " << step-prevF  << " acceptedPrefSize " << acceptedDirectionPreferences.size();
            for(map<double,int> :: iterator it = acceptedDirectionPreferences.begin(); it != acceptedDirectionPreferences.end(); it++)
               cout << " " << it->first << "->" << it->second;
            cout << " rejectedPrefSize " << rejectedDirectionPreferences.size(); 
            for(map<double,int> :: iterator it = rejectedDirectionPreferences.begin(); it != rejectedDirectionPreferences.end(); it++)
               cout << " " << it->first << "->" << it->second;
            cout << endl;
            prevF = step;
            if (isTowardWall(neighbours,currentAction)) cout << "WTF Wall!" << endl;
            cout.precision(numeric_limits< double >::digits10+1);
            cout << " lev0 bidRank size " << learnersRanked[0].size() << " [bid,esize,refs,numFeatures,gtime,aGtime,id,--]";
            for(set<learner*, LearnerBidLexicalCompare> :: iterator it = learnersRanked[0].begin(); it != learnersRanked[0].end();++it)
            {
               cout << " [" << (*it)->key() << ",";
               cout << (*it)->esize() << ",";
               cout << (*it)->refs() << ",";
               cout << (*it)->numFeatures()<< ",";
               cout << (*it)->gtime() << ",";
               cout << (*it)->ancestralGtime() << ",";
               cout << (*it)->id() << ",";
               cout << (*it)->lastCompareFactor() << "]";
            }
            cout << endl;
#endif
            behaviourSequence.push_back((double)((currentAction*-1)-1)); //actions are represented as negatives 
            getDirectedState(currentState,selectedDirectedState,currentAction);
            mspacmanDiscretizeState(selectedDirectedState, sbbEval.stateDiscretizationSteps());//only need the last 5!
            behaviourSequence.insert(behaviourSequence.end(),selectedDirectedState.begin(),selectedDirectedState.end());
            //behaviourSequence.push_back(discretize(xCoord,0,116,3));
            //behaviourSequence.push_back(discretize(yCoord,0,116,3));
            e_mspacmanServer.send("act", "i", currentAction);
            cout << endl << "ai3154 state " << vecToStr(currentState) << " " << currentAction+1 << endl;
            step++;
            decisionInstructionsSum += decisionInstructions;
            if (visual){
               e_policyAnimator.send("act","ii",winningSymbiont_ID_1,winningSymbiont_ID_0);
               cout << "winningSymbiont_ID_1 " << winningSymbiont_ID_1 << " " << winningSymbiont_ID_1_prev << endl;
               cout << "winningSymbiont_ID_0 " << winningSymbiont_ID_0 << " " << winningSymbiont_ID_0_prev << endl;
               //if (winningSymbiont_ID_1 != winningSymbiont_ID_1_prev || winningSymbiont_ID_0 != winningSymbiont_ID_0_prev){
               //   cout << "-----------------------------------------------------------" << endl;
               //   usleep(500000);
               //}
            }
            //e_mspacmanServer.send("act", "ii", pathToNearestPowerPill[ppAct++]);
            //if (ppAct > 100) ppAct = 0;
            prevAction = currentAction;
            //wait for new state from server (newState must be updated in state_handler)
            while (newState.load() == false && episodeEnd.load() < 1){ usleep(1000); } newState.store(false); 
         }
         timeGenTotalInGame += (time(NULL) - timeStartGame);
         episodeEnd.store(0);
         int start = min((1+sbbEval.dim())*MAX_BEHAVIOUR_STEPS,(int)behaviourSequence.size());
         for (int b = behaviourSequence.size()-start; b < behaviourSequence.size(); b++)
            tmpBehaviourSequence.push_back(behaviourSequence[b]);
         sbbEval.setOutcome(tmpBehaviourSequence,tmpBehaviourSequence.size(),gameScore,pillsEaten,ghostScore,e_phase,e_t);
         behaviourSequence.clear();
         tmpBehaviourSequence.clear();
         if (hostToReplay >= 0){
            cout << fixed << setprecision(4);
            cout << " gameScore " << gameScore << " pillScore " << pillsEaten << " ghostScore " << ghostScore << " steps " << step  << " meanDesicionInst " << decisionInstructionsSum/step << endl;
         }
         else
            sbbEval.getNextTeam();
      }
   }
   //cout << "mspacmanSBBAgent::runEval t " << e_t << " l " << e_level << " numProfilePoints " << sbbEval.numProfilePoints() << " newProfilePoints " << newProfilePoints << endl;
}
/***********************************************************************************************************************/
int main (int argc, char* argv[])
{
   cout.precision(numeric_limits< double >::digits10+1);

   ifstream ifs;
   ofstream ofs;
   stringstream ss;

   bool checkpoint = false;
   int checkpointInMode;
   int hostFitnessMode = 1;
   int hostToReplay;
   int levelPickup;
   int levelStart = 0;
   int numLevels = 1;
   int phase = TRAIN_PHASE;
   int port;
   long prevF = 0;
   bool replay = false;
   long seed;
   bool startNewLevel = false;
   int statMod = 1;
   long step = 0; //running count of all interactions with environment
   int tMain = 1;
   int tPickup;
   int tStart = 0;
   bool visual = false;

   // read in all the command options and change the associated variables
   // assume every two values supplied at prompt, form a duo
   cout << "mspacmanSBBAgent parameters:" << endl << endl;
   char * str;
   for( int i = 1 ; i < argc ; i = i + 1  )
   {
      if( argv[i][0] == '-' && strlen( argv[i] ) > 1)
      {
         switch( argv[i][1] )
         {
            case 'C':
               str   = &argv[i+1][0];
               checkpoint = true;
               checkpointInMode = Parse::parseFirstInt( &str );
               cout << "chechkpoint " << checkpoint << endl;
               cout << "checkPointInMode " << checkpointInMode << endl;
               break;
            case 'f':
               str   = &argv[i+1][0];
               hostFitnessMode = Parse::parseFirstInt( &str );
               cout << "hostFitnessMode " << hostFitnessMode << endl;
               break;
            case 'H':
               startNewLevel = true;
               cout << "startNewLevel " << startNewLevel << endl;
               break;
            case 'L':
               str   = &argv[i+1][0];
               numLevels = Parse::parseFirstInt( &str );
               cout << "numLevels " << numLevels << endl;
               break;
            case 'l':
               str   = &argv[i+1][0];
               levelPickup = Parse::parseFirstInt( &str );
               levelStart = levelPickup;
               cout << "levelPickup " << levelPickup << endl;
               break;
            case 'O':
               str   = &argv[i+1][0];
               statMod = Parse::parseFirstInt( &str );
               cout << "statMod " << statMod << endl;
               break;
            case 'P':
               str   = &argv[i+1][0];
               replay = true;
               hostToReplay = Parse::parseFirstInt( &str );
               cout << "replay " << replay << endl;
               cout << "hostToReplay " << hostToReplay << endl;
               break;
            case 'p':
               str   = &argv[i+1][0];
               port = Parse::parseFirstInt( &str );
               cout << "port " << port << endl;
               break;
            case 's':
               str   = &argv[i+1][0];
               seed = Parse::parseFirstInt( &str );
               cout << "seed " << seed << endl;
               break;
            case 'T':
               str   = &argv[i+1][0];
               tMain = Parse::parseFirstInt( &str );
               cout << "tMain " << tMain << endl;
               break;
            case 't':
               str   = &argv[i+1][0];
               tPickup = Parse::parseFirstInt( &str );
               tStart = tPickup;
               cout << "tPickup " << tPickup << endl;
               break;
            case 'V':
               str   = &argv[i+1][0];
               visual = true;
               cout << "visual " << visual << endl;
               break;
            case 'h':
               cout << endl << "Help" << endl << endl << "Command Line Options:" << endl << endl;
               cout << "-C <mode to read checkpoint from> (Read a checkpoint created during TAIN_MODE:0, VALIDATION_MODE:1, or TEST_MODE:2. Requires checkpoint file and options -l, and -t.)" << endl;
               cout << "-f <hostFitnessMode> (GameScore:0 Pillscore:1, Ghostscore:2)" << endl;
               cout << "-H (Invoke hierarchy imediately after reading in checkpoint. Requires checkpoint file and options -C, -l, and -t.)" << endl;
               cout << "-l <pickup level> (When loading populations form a checkpoint, 'pickup level' is the generation of the checkpoint file. Requires checkpoint file and options -C, and -t.)" << endl;
               cout << "-O <statMod> (How often to calculate and print stats)" << endl;
               cout << "-P <hostIdToReplay> (Load and replay a specific host ID. Requires checkpoint file and options -C, -l, and -t.)" << endl;
               cout << "-p <port> (Port for communicating with game server.)" << endl;
               cout << "-s <seed> (Random seed)" << endl;
               cout << "-T <generations>" << endl;
               cout << "-t <t> (When loading populations form a checkpoint, t is the generation of the checkpoint file. Requires checkpoint file and options -C, and -l.)" << endl;
               cout << "-V (Run with visualization.)" << endl << endl;
               exit(0);
               break;
            default:
               break;
         }
      }
   }
   cout << endl;

   sbbTD sbbMain;
   bool initialize = true;

   //timing 
   int timeGenSec0;
   int timeGenSec1;
   int timeGenTotalInGame;
   int timeTemp;
   int timeGenTeams;
   int timeSelTeams;
   int timeCleanup;
   //osc stuff
   lo::Address mspacmanServer("localhost", port);
   lo::Address policyAnimator("localhost", port+3);
   lo::ServerThread st(port+1);
   if (!st.is_valid()) {
      std::cout << "Nope." << std::endl;
      return 1;
   }
   st.add_method("state", NULL, state_handler, NULL); 
   st.add_method("end", NULL, end_handler, NULL);

   //SBB Parameter Setup
   sbbMain.id(-1);
   sbbMain.numLevels(numLevels);
   sbbMain.seed(seed); 
   sbbMain.dim(SBB_DIM); 
   sbbMain.setParams(); 
   sbbMain.numAtomicActions(2); //Binary action, yes or no for direction
   sbbMain.numStoredOutcomesPerHost(TRAIN_PHASE,replay ? 100 : sbbMain.episodesPerGeneration());
   sbbMain.maxTrainingReward(HUGE_VAL);
   init(currentState,neighbours,95);
   st.start();

   //loading populations from a chackpoint file
   if (checkpoint == true)
   {
      prepareInFile(ifs,"checkpoints","cp",levelPickup,tPickup,sbbMain.id(),sbbMain.seed(),checkpointInMode);
      sbbMain.readCheckpoint(checkpointInMode,ifs); ifs.close();

      ///////////////////////////////////////////////////////////////
      //sbbMain.scaleTeamAndLearnerIds(1000000);
      //sbbMain.seed(sbbMain.seed()+1000);
      //prepareOutFile(ofs,"checkpoints","cp",levelPickup,tPickup,sbbMain.id(),sbbMain.seed(),checkpointInMode);
      //sbbMain.writeCheckpoint(checkpointInMode,ofs); ofs.close();
      //return 0;


      //////////////////////////////////////////////////////////////
      sbbMain.recalculateLearnerRefs();
      sbbMain.cleanup(tPickup,levelPickup,false);// don't prune learners because active/inactive is not accurate
      //prepareOutFile(ofs,"checkpoints","cp",levelPickup,tPickup,sbbMain.id(),sbbMain.seed(),checkpointInMode);
      //sbbMain.writeCheckpoint(checkpointInMode,ofs); ofs.close();
      //return 0;

      if (replay){
         sbbMain.printTeamInfo(tPickup,levelPickup,TRAIN_PHASE);
         runEval(policyAnimator,mspacmanServer,sbbMain,tPickup,levelPickup,TRAIN_PHASE,visual,timeGenTotalInGame,hostToReplay);
         sbbMain.printTeamInfo(tPickup,levelPickup,TRAIN_PHASE);
         cout << "Goodbye cruel world." << endl;
         mspacmanServer.send("exit", "");
         return 0;
      }

      initialize = false; //can skip initialization for current level
      //starting a new level
      if (startNewLevel)
      {
         sbbMain.finalize();
         levelStart++;
         tStart = 0;
         initialize = true; //will need to initialize for the next level
      }
   }
   //hierarchical training loop
   for (int level = levelStart; level < sbbMain.numLevels(); level++)
   {
      if (initialize == true){
         timeTemp = time(NULL);
         sbbMain.initTeams(level);
         cout << "sbb::initTeamsTime sec " << time(NULL)-timeTemp << endl;
         tStart = 0;
      }
      phase = TRAIN_PHASE;
      for (int t = tStart+1; t <= tMain; t++)
      {
         timeGenSec0 = time(NULL);
         timeGenTotalInGame = 0;
         timeTemp = time(NULL); sbbMain.genTeams(t, level); timeGenTeams = time(NULL) - timeTemp; //replacement
         runEval(policyAnimator,mspacmanServer,sbbMain,t,level,phase,visual,timeGenTotalInGame,-1);
         sbbMain.hostDistanceMode(drand48() > 0.5 ? 1 : 0); //diversity switching
         sbbMain.hostFitnessMode(hostFitnessMode);
         mspacmanServer.send("sleep");
         timeTemp=time(NULL);sbbMain.selTeams(t, level);timeSelTeams=time(NULL)-timeTemp; //selection
         mspacmanServer.send("wake");
         timeTemp=time(NULL);sbbMain.cleanup(t, level,true);timeCleanup=time(NULL)-timeTemp;
         prepareOutFile(ofs,"checkpoints","cp",level,t,sbbMain.id(),sbbMain.seed(),TRAIN_PHASE);
         sbbMain.writeCheckpoint(TRAIN_PHASE, ofs); ofs.close();
         sbbMain.printTeamInfo(t,level,phase);
         timeGenSec1 = time(NULL);
         cout << "sbb::genTime t " << t  << " sec " << timeGenSec1 - timeGenSec0 << " (" << timeGenTotalInGame << " InGame, ";
         cout << timeGenTeams << " genTeams, " << timeSelTeams << " selTeams, " << timeCleanup << " cleanup)" << endl;
         if (t == tStart+1 || t % statMod == 0 || t == tMain){
            sbbMain.stats(t, level);
         }
      }
      sbbMain.finalize();
      initialize = true;
   }
   sbbMain.finalfinalize();
   cout << "Goodbye cruel world." << endl;
   mspacmanServer.send("exit", ""); 
   return 0;
}

