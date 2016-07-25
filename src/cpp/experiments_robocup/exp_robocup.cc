#include <string.h>  // for memcpy
#include <stdlib.h>  // for rand
#include <iterator>
#include <time.h>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include "Parse.h"
#include <limits>
#include "exp_robocup.h"
#include "../sbbTD/soccer.h"
using std::numeric_limits;
using namespace std;

int main (int argc, char* argv[])
{
   // read in all the command options and change the associated variables
   // assume every two values supplied at prompt, form a duo
   char * str;
   for( int i = 1 ; i < argc ; i = i + 1  )
   {
      if( argv[i][0] == '-' && strlen( argv[i] ) > 1)
      {
         switch( argv[i][1] )
         {
            case 'A':
               str   = &argv[i+1][0];
               sizeTeamA = Parse::parseFirstInt( &str );
               break;
            case 'B':
               str   = &argv[i+1][0];
               sizeTeamB = Parse::parseFirstInt( &str );
               break;
            case 'C':
               str   = &argv[i+1][0];
               checkpoint = true;
               checkpointInMode = Parse::parseFirstInt( &str );
               break;
            case 'D':
               str   = &argv[i+1][0];
               initialDistanceToGoal = Parse::parseFirstInt( &str );
               break;
            case 'E':
               str   = &argv[i+1][0];
               testPhaseMod = Parse::parseFirstInt( &str );
               break;
            case 'e':
               str   = &argv[i+1][0];
               iLearn = Parse::parseFirstInt( &str );
               break;
            case 'f':
               str   = &argv[i+1][0];
               numFeatures = Parse::parseFirstInt( &str );
               break;
            case 'G':
               save_kwy_log = true; 
               break;
            case 'g':
               str   = &argv[i+1][0];
               numGamesToReplay = Parse::parseFirstInt( &str );
               break;
            case 'H':
               startNewLevel = true;
               break;
            case 'i':
               str   = &argv[i+1][0];
               playerIdToReplay = Parse::parseFirstInt( &str );
               break;
            case 'L':
               str   = &argv[i+1][0];
               numLevels = Parse::parseFirstInt( &str );
               break;
            case 'l':
               str   = &argv[i+1][0];
               levelStart = Parse::parseFirstInt( &str );
               break;
            case 'm':
               monitor = 1; 
               break;
            case 'N':
               synch_mode = 0;
               break;
            case 'O':
               str   = &argv[i+1][0];
               statMod = Parse::parseFirstInt( &str );
               cout << "statMod " << statMod << endl;
               break;
            case 'p':
               str   = &argv[i+1][0];
               phaseToReplay = Parse::parseFirstInt( &str );
               replayMode = true; 
               break;
            case 'Q':
               str   = &argv[i+1][0];
               numTestPhases = Parse::parseFirstInt( &str );
               break;
            case 'R':
               rcss_noise = true;
               break;
            case 'S':
               policy = SARSA_POLICY;
               str   = &argv[i+1][0];
               sarsaEpochs = Parse::parseFirstInt( &str );
               break;
            case 's':
               str   = &argv[i+1][0];
               seed = Parse::parseFirstInt( &str );
               break;
            case 'T':
               str   = &argv[i+1][0];
               tMain = Parse::parseFirstInt( &str );
               break;            
            case 't':
               str   = &argv[i+1][0];
               tStart = tPickup = Parse::parseFirstInt( &str );
               break;
            case 'V':
               str   = &argv[i+1][0];
               varify_policy_number = Parse::parseFirstInt( &str );
               break;
            case 'W':
               str   = &argv[i+1][0];
               sarsaWeightFileEpochNum = Parse::parseFirstInt( &str );
               break;          
            case 'Y':
               str   = &argv[i+1][0];
               taskType = Parse::parseFirstInt( &str );
               break;
            case 'h':
               cout << "Command Line Options:" << endl;
               cout << "-A <sizeTeamA> (Default 3)" << endl;
               cout << "-B <sizeTeamB>  (Default 2)" << endl;
               cout << "-C <mode to read checkpoint from> (Read a checkpoint created during TAIN_MODE:0, VALIDATION_MODE:1, or TEST_MODE:2. Requires checkpoint file and options -l, and -t.)" << endl;
               cout << "-D <initail distance to goal> (Scoring task:0 Half Field Offense:1)" << endl;
               cout << "-e <sarsa learning> (0:off 1:on, Off requires option -W)" << endl;
               cout << "-f <numFeautres>  (Number of sensor inputs for the task)" << endl;
               cout << "-G (save_kwy_log)" << endl;
               cout << "-g <numGamesToReplay> (How many games for policy replay)" << endl;
               cout << "-i <playerIdToReplay> (ID of policy to load)" << endl;
               cout << "-l <pickup level> (When loading populations form a checkpoint, 'pickup level' is the generation of the checkpoint file. Requires checkpoint file and options -C, and -t.)" << endl;
               cout << "-m: (Visualize with rcssmonitor)" << endl;
               cout << "-N (synch_mode off, run slower for visualization)" << endl;
               cout << "-p <phase to replay> (Replay a policy and specify which phase the checkpoint is from, TAIN_MODE:0, VALIDATION_MODE:1, or TEST_MODE:2.)" << endl;
               cout << "-Q <numTestPhases>" << endl;
               cout << "-R (Turn rcss_noise on)" << endl;
               cout << "-S (Use Sarsa policy)" << endl;
               cout << "-s <Random Seed>" << endl;    
               cout << "-t <t> (When loading populations form a checkpoint, t is the generation of the checkpoint file. Requires checkpoint file and options -C, and -l.)" << endl;
               cout << "-V <varify_policy_number>" << endl;   
               cout << "-W <sarsa weight file epoch num> (Load a Sarsa weight file.)" << endl;
               cout << "-Y <taskType> (1:half-field 3:keepaway)" << endl;
               exit(0);
               break;
            default:
               cerr << "(main) Unknown command option: " << argv[i] << endl;
               break;
         }
      }
   }

   /***********************************************************************************************************************/
   //parameter setup
   if (numFeatures == 0)
      die(__FILE__, __FUNCTION__, __LINE__, "numFeatures not set!");
   sbb.seed(seed);
   sbb.dim(numFeatures);
   sbb.numLevels(numLevels);
   sbb.id(1);
   sbb.setParams();
   if (taskType == 1)
      sbb.numAtomicActions(sizeTeamA+1);
   else if (taskType == 3)
      sbb.numAtomicActions(sizeTeamA);
   sbb.numStoredOutcomesPerHost(TRAIN_PHASE,2*sizeTeamA*sbb.episodesPerGeneration());
   sbb.numStoredOutcomesPerHost(VALIDATION_PHASE,(sizeTeamA*sbb.validPhaseEpochs())/2);
   sbb.numStoredOutcomesPerHost(TEST_PHASE,sizeTeamA*sbb.testPhaseEpochs());
   sbb.numStoredOutcomesPerHost(PLAY_PHASE,sizeTeamA*sbb.testPhaseEpochs());
   sbb.maxTrainingReward(2.0);
   rcssNoise(rcss_noise);
   /***********************************************************************************************************************/
   //sarsa
   if (policy == SARSA_POLICY)
   {
      sbb.initTeams(0);
      sbb.diversityMode(0);
      sbb.genTeams(1, 0);
      sbb.makeEvaluationVector(1,TRAIN_PHASE,false);
      prepareOutFile(ofs,"checkpoints","cp",-1,-1,sbb.id(),sbb.seed(),TRAIN_PHASE);
      sbb.writeCheckpoint(TRAIN_PHASE, ofs); ofs.close();
      runEval(-1,-1,sarsaEpochs,TRAIN_PHASE,sizeTeamA,sizeTeamB,policy);
      cout << "Goodbye cruel world. (Sarsa)" << endl;
      return 0;
   }
   /***********************************************************************************************************************/
   //replay a policy from file
   if (replayMode == true)
   {
      prepareInFile(ifs,"checkpoints","cp",levelPickup,tPickup,playerIdToReplay,sbb.seed(),phaseToReplay);
      sbb.readCheckpoint(phaseToReplay,ifs); ifs.close();
      prepareOutFile(ofs,"checkpoints","cp",levelPickup,tPickup,sbb.id(),sbb.seed(),PLAY_PHASE);
      sbb.writeCheckpoint(PLAY_PHASE,ofs); ofs.close();
      sbb.makeEvaluationVector(0,PLAY_PHASE,false,numGamesToReplay);

      //if (varify_policy_number > 0){
      //   sbb.getFirstTeam();
      //   varifyPolicy(varify_policy_number);
      //}
      sbb.printTeamInfo(tStart,levelStart,PLAY_PHASE);
      runEval(tStart,levelStart,sbb.evaluationVectorSize(),PLAY_PHASE,sizeTeamA,sizeTeamB,policy);
      for (int i = 1; i <= sizeTeamA; i++)
         sbb.processEvalResults(tPickup,levelPickup,PLAY_PHASE,i);
      sbb.cleanup(tStart,levelStart,true);
      sbb.printTeamInfo(tStart,levelStart,PLAY_PHASE);
      cout << "Goodbye cruel world. (Replay)" << endl;
      return 0;
   }

   /***********************************************************************************************************************/
   //initialize from checkpoint
   if (checkpoint == true)
   {
      prepareInFile(ifs,"checkpoints","cp",levelPickup,tPickup,sbb.id(),sbb.seed(),checkpointInMode);
      sbb.readCheckpoint(checkpointInMode,ifs); ifs.close();
      ////re-validate
      //sbb.hostDistanceMode(1);
      //sbb.hostFitnessMode(TRAIN_REWARD);
      //sbb.makeEvaluationVector(tPickup,VALIDATION_PHASE,false);
      //if (sbb.evaluationVectorSize() > 0){
      //   prepareOutFile(ofs,"checkpoints","cp",levelPickup,tPickup,sbb.id(),sbb.seed(),VALIDATION_PHASE);
      //   sbb.writeCheckpoint(VALIDATION_PHASE, ofs); ofs.close();
      //   runEval(tPickup,levelPickup,sbb.evaluationVectorSize(),VALIDATION_PHASE,sizeTeamA,sizeTeamB, policy);
      //   for (int i = 1; i <= sizeTeamA; i++)
      //      sbb.processEvalResults(tPickup,levelPickup,VALIDATION_PHASE,i);
      //}
      //sbb.paretoScoreRanking(tPickup,levelPickup,VALIDATION_PHASE);
      //return 0;

      //////////////////////////////////////////////////////////////
      //sbb.recalculateLearnerRefs();
      //sbb.cleanup(tPickup,levelPickup,false);// don't prune learners because they active/inactive is not accurate
      //prepareOutFile(ofs,"checkpoints","cp",levelStart,tStart,sbb.id(),sbb.seed(),checkpointInMode);
      //sbb.writeCheckpoint(checkpointInMode,ofs); ofs.close();
      //return 0;

      ///////////////////////////////////////////////////////////////
      //sbb.scaleTeamAndLearnerIds(1000000);
      //sbb.seed(sbb.seed()+1000);
      //prepareOutFile(ofs,"checkpoints","cp",levelPickup,tPickup,sbb.id(),sbb.seed(),checkpointInMode);
      //sbb.writeCheckpoint(checkpointInMode,ofs); ofs.close();
      //return 0;

      ////////////////////////////////////////////////////////////
      //set <learner*> lp;
      //sbb.getLearnerPop(lp);
      //mapKeepawayToHalfFieldActions(lp);
      //sbb.replaceLearnerPop(lp);
      //prepareOutFile(ofs,"checkpoints","cp",levelPickup,tPickup,sbb.id(),sbb.seed(),VALIDATION_PHASE);
      //sbb.writeCheckpoint(VALIDATION_PHASE,ofs); ofs.close();
      //return 0;     

      initialize = false; //can skip initialization for current level

      /***********************************************************************************************************************/
      //starting a new level
      if (startNewLevel)//&& tStart >= sbb.t())
      { 
         sbb.finalize();
         levelStart++;
         tStart = 0;
         initialize = true; //will need to initialize for the next level
      }
   }
   /***********************************************************************************************************************/
   //hierarchical training loop
   for (level = levelStart; level < numLevels; level++)
   {
      if (initialize){
         timeTemp=time(NULL);sbb.initTeams(level);timeInitTeams=time(NULL)-timeTemp;
         tStart = 0;
      }
      //train
      for (int t = tStart+1; t <= tMain; t++)
      {
         timeGeneration = time(NULL);
         //replacement
         timeTemp=time(NULL);sbb.genTeams(t, level);timeGenTeams=time(NULL)-timeTemp;
         //evaluate
         timeTemp=time(NULL);
         sbb.makeEvaluationVector(t,TRAIN_PHASE,false);
         cout << "soc::numEval t " << t << " level " << level << " evalSize " << sbb.evaluationVectorSize() << endl;
         if (sbb.evaluationVectorSize() > 0){
            prepareOutFile(ofs,"checkpoints","cp",level,t,sbb.id(),sbb.seed(),TRAIN_PHASE);
            sbb.writeCheckpoint(TRAIN_PHASE,ofs);ofs.close();
            runEval(t,level,sbb.evaluationVectorSize(),TRAIN_PHASE,sizeTeamA,sizeTeamB,policy); 
            for (int i = 1; i <= sizeTeamA; i++)
               sbb.processEvalResults(t,level,TRAIN_PHASE,i);
         }
         timeEvaluate=time(NULL)-timeTemp;

         //if multi-level, switch off diversity half-way through the last level
         //if (numLevels > 1 && ( t>= tMain/2 && level == numLevels-1))
         //   sbb.diversityMode(0);           
         //diversity switching
         sbb.hostDistanceMode((int)(drand48()*2)); //0 or 1
         //fitness switching
         sbb.hostFitnessMode(TRAIN_REWARD);
         //selection
         timeTemp=time(NULL);sbb.selTeams(t, level);timeSelTeams=time(NULL)-timeTemp;
         sbb.cleanup(t, level, true);
         //save selected population
         prepareOutFile(ofs,"checkpoints","cp",level,t,sbb.id(),sbb.seed(),-1);
         sbb.writeCheckpoint(TRAIN_PHASE,ofs);ofs.close();
         //stats
         if (statMod > 0 && (t == tStart+1 || t % statMod == 0 || t == tMain)){
            timeTemp=time(NULL);sbb.stats(t, level);sbb.printTeamInfo(t,level,TRAIN_PHASE);timeStats=time(NULL)-timeTemp;
         } else timeStats = 0;
         //log generation timing
         oss << "soc::genTime t " << t  << " sec " << time(NULL) - timeGeneration << " initTeams " << timeInitTeams << " genTeams " << timeGenTeams; 
         oss << " eval " << timeEvaluate << " selTeams " << timeSelTeams << " stats " << timeStats;
         oss << " other " << (time(NULL) - timeGeneration) - (timeInitTeams+timeGenTeams+timeEvaluate+timeSelTeams+timeStats);
         cout << oss.str() << endl;
         oss.str("");

         //test
         if (testPhaseMod > 0 && ((t == tStart+1 || t == tMain) || t % testPhaseMod == 0))
            runTest(t,level);
         if (t >= (tMain - numTestPhases))
            runTest(t,level);
      }
      sbb.finalize();
      initialize = true;
   }
   sbb.finalfinalize();
   cout << "Goodbye cruel world. (Main)" << endl;
   return 0;
}
