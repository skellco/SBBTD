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

int sbbSoccer(int lS, int tS)
{
   int levStart = lS;
   int genStart = tS;
   sbb.clear(); //clear host and symbiont populations before we start
   /***********************************************************************************************************************/
   //parameter setup
   if (numFeatures == 0)
      die(__FILE__, __FUNCTION__, __LINE__, "numFeatures not set!");
   sbb.seed(seed);
   sbb.dim(numFeatures);
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
      prepareInFile(ifs,"checkpoints","cp",levStart,genStart,playerIdToReplay,sbb.seed(),phaseToReplay);
      sbb.readCheckpoint(phaseToReplay,ifs); ifs.close();
      prepareOutFile(ofs,"checkpoints","cp",levStart,genStart,sbb.id(),sbb.seed(),PLAY_PHASE);
      sbb.writeCheckpoint(PLAY_PHASE,ofs); ofs.close();
      sbb.makeEvaluationVector(0,PLAY_PHASE,false,numGamesToReplay);

      //if (varify_policy_number > 0){
      //   sbb.getFirstTeam();
      //   varifyPolicy(varify_policy_number);
      //}
      sbb.printTeamInfo(genStart,levStart,PLAY_PHASE);
      runEval(genStart,levStart,sbb.evaluationVectorSize(),PLAY_PHASE,sizeTeamA,sizeTeamB,policy);
      for (int i = 1; i <= sizeTeamA; i++)
         sbb.processEvalResults(genStart,levStart,PLAY_PHASE,i);
      sbb.cleanup(genStart,levStart,true);
      sbb.printTeamInfo(genStart,levStart,PLAY_PHASE);
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
      //prepareOutFile(ofs,"checkpoints","cp",levelStart,genStart,sbb.id(),sbb.seed(),checkpointInMode);
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
      if (startNewLevel)//&& genStart >= sbb.t())
      { 
         sbb.finalize();
         levStart++;
         genStart = 0;
         initialize = true; //will need to initialize for the next level
      }
   }
   /***********************************************************************************************************************/
   //hierarchical training loop
   for (level = levStart; level < sbb.numLevels(); level++)
   {
      if (initialize){
         timeTemp=time(NULL);sbb.initTeams(level);timeInitTeams=time(NULL)-timeTemp;
         genStart = 0;
      }
      //train
      for (int t = genStart+1; t <= sbb.t(); t++)
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
         if (sbb.numLevels() > 1 && ( t>= sbb.t()/2 && level == sbb.numLevels()-1))
            sbb.diversityMode(0);           
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
         if (statMod > 0 && (t == genStart+1 || t % statMod == 0 || t == sbb.t())){
            timeTemp=time(NULL);sbb.stats(t, level);sbb.printTeamInfo(t,level,TRAIN_PHASE);timeStats=time(NULL)-timeTemp;
         } else timeStats = 0;
         //log generation timing
         oss << "soc::genTime t " << t  << " sec " << time(NULL) - timeGeneration << " initTeams " << timeInitTeams << " genTeams " << timeGenTeams; 
         oss << " eval " << timeEvaluate << " selTeams " << timeSelTeams << " stats " << timeStats;
         oss << " other " << (time(NULL) - timeGeneration) - (timeInitTeams+timeGenTeams+timeEvaluate+timeSelTeams+timeStats);
         cout << oss.str() << endl;
         oss.str("");

         //test
         if (testPhaseMod > 0 && ((t == genStart+1 || t == sbb.t()) || t % testPhaseMod == 0))
            runTest(t,level);
      }
      sbb.finalize();
      initialize = true;
   }
   sbb.finalfinalize();
   return 0;
}

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
            case 'I':
               str   = &argv[i+1][0];
               islandIn = Parse::parseFirstInt( &str );
               break;
            case 'i':
               str   = &argv[i+1][0];
               playerIdToReplay = Parse::parseFirstInt( &str );
               break;
            case 'L':
               str   = &argv[i+1][0];
               levelPickup = Parse::parseFirstInt( &str );
               break;
            case 'l':
               str   = &argv[i+1][0];
               levelStart = Parse::parseFirstInt( &str );
               break;
            case 'm':
               //str   = &argv[i+1][0];
               monitor = 1; //Parse::parseFirstInt( &str );
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
               replayMode = true; //Parse::parseFirstInt( &str );
               break;
            case 'R':
               rcss_noise = true;
               break;
            case 's':
               str   = &argv[i+1][0];
               seed = Parse::parseFirstInt( &str );
               break;
            case 'S':
               policy = SARSA_POLICY;
               str   = &argv[i+1][0];
               sarsaEpochs = Parse::parseFirstInt( &str );
               break;            
            case 'T':
               str   = &argv[i+1][0];
               tPickup = Parse::parseFirstInt( &str );
               break;
            case 't':
               str   = &argv[i+1][0];
               tStart = Parse::parseFirstInt( &str );
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
               cout << "-s <seed>" << endl;
               cout << "-m: (monitor on, default:off)" << endl;
               cout << "-C <phase to read checkpoint from> (default:off, requires -T and -L)" << endl;
               cout << "Note on starting from checkpoint: if starting a new level, set -t <generation to start from in previous level (usually last gen)>" << "and -l <previous level>" << endl; 
               cout << "-T <pickup generation> (default:1, requires -L and checkpoint file!)" << endl;
               cout << "-L <pickup level> (default:0, requires -T and checkpoint file!)" << endl;
               cout << "-p <phase to replay> (default:off, phaseToReplay default:2, requires:-t,-l,-i,-g)" << endl;
               cout << "-t <t> (what generation to start from OR playback policy is from)" << endl;
               cout << "-l <level> (what level to start from OR playback policy is from)" << endl;
               cout << "-i <playerIdToReplay> (id of policy to load)" << endl;
               cout << "-g <numGamesToReplay> (how many games for policy replay)" << endl;
               cout << "-A <sizeTeamA> (default:3)" << endl;
               cout << "-B <sizeTeamB>  (default:2)" << endl;
               cout << "-f <numFeautres>  (required)" << endl;
               cout << "-I <islandIn> (default:0)" << endl;
               cout << "-G (save_kwy_log default:off)" << endl;
               cout << "-V <varify_policy_number> (default:0)" << endl;
               cout << "-S (use Sarsa policy, default:off)" << endl;
               cout << "-Y <taskType> (1:half-field 3:keepaway, default:1)" << endl;
               cout << "-e <sarsa learning> (0:off and read weight file, 1:on, default:on, off requires -W)" << endl;
               cout << "-W <sarsa weight file epoch num> (requires -e 0)" << endl;
               cout << "-N (synch_mode off, default:on)" << endl;
               cout << "-R (turn rcss_noise on, default:off)" << endl;
               exit(0);
               break;
            default:
               cerr << "(main) Unknown command option: " << argv[i] << endl;
               break;
         }
      }
   }
   while (sbbSoccer(levelStart, tStart));
   cout << "Goodbye cruel world. (Main)" << endl;
   return 0;
}