#ifndef MAIN_H
#define MAIN_H
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include "../sbbTD/sbbTD.h"
#include <ctime>

#define SBB_POLICY 0
#define SARSA_POLICY 1

using namespace std;

sbbTD sbb;
ostringstream oss;
ifstream ifs;
ofstream ofs;
char inputFilename[80];
char outputFilename[80];
//globals for potential command-line parameters
bool checkpoint = false;
int checkpointInMode = 1;
int iLearn = 1;
bool initialDistanceToGoal = 1.0;
bool initialize = true;
long level;
int levelPickup = 0;
int levelStart = 0;
int phaseToReplay = 2;
bool monitor = false;
int numFeatures;
int numGamesToReplay;
int numLevels = 1;
int numTestPhases = 1;
int playerIdToReplay;
int policy = 0;
bool replayMode = false;
int sarsaEpochs = 0;
int sarsaWeightFileEpochNum = 0;
bool save_kwy_log = false;
int sizeRunningMaxMean = 4;
int sizeTeamA = 3;
int sizeTeamB = 2;
int statMod = 1;
bool startNewLevel = false;
int synch_mode=1;
long seed = 0;
int taskType = 1;
int testPhaseMod = 1;
int tMain = 1;
int tPickup = 0;
int tStart = 0;
bool rcss_noise = false;
int varify_policy_number = 0;

//globals for timing
int timeTemp;
int timeGeneration;
int timeInitTeams;
int timeGenTeams;
int timeSelTeams;
int timeEvaluate;
int timeStats;
double sys1, usr1, sys2, usr2;

//globals for switchable noise parameters
double rcss_quantize_step=0;
double rcss_quantize_step_l=0;
double rcss_ball_rand=0;
double rcss_kick_rand=0;
double rcss_kick_rand_factor_l=0;
double rcss_kick_rand_factor_r=0;
double rcss_player_rand=0;
double rcss_prand_factor_l=0;
double rcss_prand_factor_r=0;
double rcss_tackle_rand_factor=0;
double rcss_wind_rand=0;

/***********************************************************************************************************************/
/*
 * SBBAgent.cc will only store evaluation otcomes that correspond to a valid endEpisode value,
 * which only happens if the player touches the ball at least once.
 * This function finds the evalution file with the most valid outcomes.
 */
int findLargestEvalFile(int phase){
   const int MAXLINE=102400;
   ifstream inFile;
   char inputFilename[80];
   int numEvalOutcomes[sizeTeamA];
   const int N = sizeof(numEvalOutcomes) / sizeof(int);
   for (int i = sizeTeamA-1; i >= 0; i--){
      oss.str("");
      oss << "evals/eval." << phase << "." << i+1 << "." << sbb.seed() << ".rslt";
      sprintf( inputFilename,"%s", oss.str().c_str());
      oss.str("");
      oss << "Can't open eval file read: " << inputFilename << endl;
      inFile.open(inputFilename, ios::in);
      if (!inFile) {
         die(__FILE__, __FUNCTION__, __LINE__, oss.str().c_str());
      }
      oss.str("");
      string line;
      numEvalOutcomes[i]=0;
      while (getline(inFile, line) )
         numEvalOutcomes[i]++; 
      inFile.close();
   }
   return 1+distance(numEvalOutcomes, max_element(numEvalOutcomes, numEvalOutcomes+N));
}
/***********************************************************************************************************************/
void startServer(int phase){
   //time_t t = time(0);   // get time now
   //struct tm * now = localtime( & t );
   //ostringstream stamp;
   //stamp << now->tm_year + 1900 << now->tm_mon + 1 << now->tm_mday << now->tm_sec;
   oss.str("");
   oss << "rcssserver";
   oss << " server::port=" << sbb.seed();
   oss << " server::coach_port=" << sbb.seed()+1;
   oss << " server::olcoach_port=" << sbb.seed()+2;
   oss << " server::half_time=-1";
   oss << " server::forbid_kick_off_offside=0";
   oss << " server::use_offside=0";
   oss << " server::stamina_inc_max=3500";
   oss << " server::stamina_capacity=-1";
   oss << " server::synch_mode=" << synch_mode;
   oss << " server::keepaway=0";
   oss << " server::keepaway_start=4";
   oss << " server::keepaway_width=" << SOCCER_FIELD_SIZE;
   oss << " server::keepaway_length=" << SOCCER_FIELD_SIZE;
   oss << " server::keepaway_logging=0";
   oss << " server::keepaway_log_dir=" << "./" << "logs";
   oss << " server::keepaway_log_fixed=1";
   oss << " server::keepaway_log_fixed_name="  << "kwy_log";
   oss << " server::game_logging=0";
   oss << " server::game_log_dir=" << "./" << "logs";
   oss << " server::game_log_compression=0";
   oss << " server::game_log_version=3";
   oss << " server::game_log_fixed=1";
   oss << " server::game_log_fixed_name=201407131234-aury";
   oss << " server::text_logging=0";
   oss << " server::text_log_dir=" << "./" << "logs";
   oss << " server::text_log_compression=0";
   oss << " server::text_log_fixed=1";
   oss << " server::text_log_fixed_name=" << "test";
   oss << " server::visible_angle=360";
   oss << " server::quantize_step=" << rcss_quantize_step;
   oss << " server::quantize_step_l=" << rcss_quantize_step_l;
   oss << " server::team_actuator_noise=false";
   oss << " server::wind_random=false";
   oss << " server::ball_rand=" <<  rcss_ball_rand;
   oss << " server::kick_rand=" << rcss_kick_rand;
   oss << " server::kick_rand_factor_l=" <<  rcss_kick_rand_factor_l;
   oss << " server::kick_rand_factor_r=" << rcss_kick_rand_factor_r;
   oss << " server::player_rand=" << rcss_player_rand;
   oss << " server::prand_factor_l=" <<  rcss_prand_factor_l;
   oss << " server::prand_factor_r=" << rcss_prand_factor_r;
   oss << " server::tackle_rand_factor=" << rcss_tackle_rand_factor;
   oss << " server::wind_rand=" <<  rcss_wind_rand;
   oss << " server::coach=1";
   oss << " server::say_coach_msg_size=512";
   oss << " server::say_msg_size=512";
   oss << " server::back_passes=0";
   oss << " > server_out &";
   system ( oss.str().c_str());
   oss.str("");
   sleep(2);
}
/***********************************************************************************************************************/
void stopServer(){
   oss.str("");
   oss << "ps t | awk '$6 ~ /Trainer -port " << sbb.seed()+1 << "/ { print $1 }' | xargs -r kill -9 ";
   system ( oss.str().c_str());
   oss.str("");
   oss << "ps t | awk '$6 ~ /server::port=" << sbb.seed() << "/ { print $1 }' | xargs -r kill -9 ";
   system ( oss.str().c_str());
   oss.str("");
   oss << "ps t | awk '$6 ~ /soccer_player -p " << sbb.seed() << "/ { print $1 }' | xargs -r kill -9 ";
   system ( oss.str().c_str());
   oss.str("");
   sleep(3);
}
/***********************************************************************************************************************/
void startMonitor(){
   oss.str("");
   oss << "rcssmonitor --server-port=" << sbb.seed() << " &";
   system ( oss.str().c_str());
   oss.str("");
}
/***********************************************************************************************************************/
void stopMonitor(){
   system ( "killall rcssmonitor" );
   oss.str("");
}
/***********************************************************************************************************************/
void rcssNoise(bool noise){
   rcss_quantize_step=noise==true?0.1:0;
   rcss_quantize_step_l=noise==true?0.01:0.00001;
   rcss_ball_rand=noise==true?0.05:0;
   rcss_kick_rand=noise==true?0.1:0;
   rcss_kick_rand_factor_l=noise==true?1:0;
   rcss_kick_rand_factor_r=noise==true?1:0;
   rcss_player_rand=noise==true?0.1:0;
   rcss_prand_factor_l=noise==true?1:0;
   rcss_prand_factor_r=noise==true?1:0;
   rcss_tackle_rand_factor=noise==true?2:0;
   rcss_wind_rand=noise==true?0:0;
}
/***********************************************************************************************************************/
void runEval(int t, int level, int evalEpisodes, int phase, int sizeTeamA, int sizeTeamB,int policy){
   mkdir("player_outs", S_IRWXU|S_IRGRP|S_IXGRP);
   startServer(phase);
   if (monitor == true) startMonitor();
   //start keepers
   for (int i = 1; i <= sizeTeamA; i++){
      oss << "../build/release/cpp/players/soccer_player";
      oss << " -P " << policy;
      oss << " -p " << sbb.seed();
      oss << " -z " << sbb.seed();
      oss << " -b " << i;
      oss << " -x " << evalEpisodes;
      oss << " -Z " << t;
      oss << " -Y " << level;
      oss << " -X " << phase;
      oss << " -f sarsa." << sbb.seed() << ".wts";
      oss << " -e " << iLearn;
      if (iLearn == 0)
         oss << " -w sarsa_weights/sarsa." << i << ".sarsa." << sbb.seed() << ".wts." << sarsaWeightFileEpochNum << ".rslt"; 
      if (taskType == 1){
         oss << " -t Offense";
         oss << " -k " << sizeTeamB;
         oss << " -j " << sizeTeamA;
      }
      else if (taskType == 3){
         oss << " -t keepers ";
         oss << " -k " << sizeTeamA;
         oss << " -j " << sizeTeamB;
      }
      oss << " -e 1";
      oss << " -q learned";
      oss << " -T " << taskType;
      oss << " -K 1";
      oss << " -R 0";
      oss << " -S 0";
      oss << " -D";
      oss << " -L";
      oss << " -A  Dummy";
      oss << " > player_outs/k" << i << "_out" << sbb.seed() << ".rslt 2>&1 &";
      system ( oss.str().c_str() );
      oss.str("");
      sleep(2);
   }
   //start takers
   for (int i = 1; i <= sizeTeamB; i++){
      if (i < sizeTeamB){
         oss << "../build/release/cpp/players/soccer_player";
         oss << " -p " << sbb.seed();
         oss << " -z " << sbb.seed();
         if (taskType == 1){ 
            oss << " -t Defense";
            oss << " -k " << sizeTeamB;
            oss << " -j " << sizeTeamA;
         }
         else if (taskType == 3){ 
            oss << " -t takers";
            oss << " -k " << sizeTeamA;
            oss << " -j " << sizeTeamB;
         }
         oss << " -b " << i;
         oss << " -e 0";
         oss << " -q hand";
         oss << " -T " << taskType;
         oss << " -g 0";
         oss << " -x " << evalEpisodes;
         oss << " -Z " << t;
         oss << " -Y " << level;
         oss << " -X " << phase;
         oss << " > player_outs/t" << i << "_out" << seed << ".rslt 2>&1 &";
      }
      else{
         if (taskType == 1){ //goalie
            oss << "../build/release/cpp/players/soccer_player";
            oss << " -p " << sbb.seed();
            oss << " -z " << sbb.seed();
            oss << " -k " << sizeTeamB;
            oss << " -j " << sizeTeamA;
            oss << " -t Defense";
            oss << " -b " << i;
            oss << " -e 0";
            oss << " -q hand";
            oss << " -T " << taskType;
            oss << " -g 1";
            oss << " -x " << evalEpisodes;
            oss << " -Z " << t;
            oss << " -Y " << level;
            oss << " -X " << phase;
            oss << " > player_outs/t" << i << "_out" << seed << ".rslt 2>&1 &";
         }
         else if (taskType == 3){//last player
            oss << "../build/release/cpp/players/soccer_player";
            oss << " -p " << sbb.seed();
            oss << " -z " << sbb.seed();
            oss << " -t takers";
            oss << " -k " << sizeTeamA;
            oss << " -j " << sizeTeamB;
            oss << " -e 0";
            oss << " -q hand";
            oss << " -T " << taskType;
            oss << " -g 0";
            oss << " -x " << evalEpisodes;
            oss << " -Z " << t;
            oss << " -Y " << level;
            oss << " -X " << phase;
            oss << " > player_outs/t" << sizeTeamB << "_out" << seed << ".rslt 2>&1 &";
         }
      }
      system ( oss.str().c_str() );
      oss.str("");
      sleep(1);
   }
   //start trainer
   oss << "java -Xms256m";
   oss << " -Xmx4g";
   oss << " -classpath ../src/java/Trainer/";
   oss << " Trainer";
   oss << " -port " << sbb.seed()+1;
   oss << " -monitor 0 ";
   if (taskType == 1){
      oss << " -keepers " << sizeTeamB;
      oss << " -takers " << sizeTeamA;
      oss << " -taskType 1";
   }	
   else if (taskType == 3){
      oss << " -keepers " << sizeTeamA;
      oss << " -takers " << sizeTeamB;
      oss << " -taskType 3";
   }
   oss << " -bounce 0";
   oss << " -elasticity 0.9";
   oss << " -torus 0";
   oss << " -move_speed 0.25";
   oss << " -kwy ./trainer-" << sbb.seed()+1 << ".hf";
   oss << " -startEpisode 0";
   oss << " -width " << SOCCER_FIELD_SIZE;
   oss << " -length " << SOCCER_FIELD_SIZE;
   oss << " -taskAdjust " << initialDistanceToGoal;
   system (oss.str().c_str());
   oss.str("");

   sleep(2);
   if (monitor == true) stopMonitor();
   stopServer();
   sleep(2);
}
/***********************************************************************************************************************/
void runTest(int t, int level){
   //// validation: ensures that each host has played at least _validationEpochs games
   //   if(getusage(sys1, usr1) == false)
   //      die(__FILE__, __FUNCTION__, __LINE__, "cannot get rusage");
   //   timeTemp = time(NULL);
   //   sbb.resetOutcomes(VALIDATION_PHASE);
   //   sbb.makeEvaluationVector(t,VALIDATION_PHASE,false);
   //   if (sbb.evaluationVectorSize() > 0){
   //      prepareOutFile(ofs,"checkpoints","cp",level,t,sbb.id(),sbb.seed(),VALIDATION_PHASE);
   //      sbb.writeCheckpoint(VALIDATION_PHASE, ofs); ofs.close();
   //      runEval(t,level,sbb.evaluationVectorSize(),VALIDATION_PHASE,sizeTeamA,sizeTeamB, policy);
   //      for (int i = 1; i <= sizeTeamA; i++)
   //         sbb.processEvalResults(t,level,VALIDATION_PHASE,i);
   //   }
   //   //log validation timing
   //   if(!getusage(sys2, usr2))
   //      die(__FILE__, __FUNCTION__, __LINE__, "cannot get rusage");

   //   oss << "soc::rusage valid t " << t << " l " << level;
   //   oss << " sys1 " << sys1 << " usr1 " << usr1 << " sum1 " << (sys1 + usr1);
   //   oss << " sys2 " << sys2 << " usr2 " << usr2 << " sum2 " << (sys2 + usr2);
   //   oss << " validtime " << (sys2 + usr2) - (sys1 + usr1) << " validSec " << time(NULL) - timeTemp;
   //   cout << oss.str() << endl;
   //   oss.str("");
   //test
   if(!getusage(sys1, usr1))
      die(__FILE__, __FUNCTION__, __LINE__, "cannot get rusage");
   timeTemp = time(NULL);
   sbb.getBestTeam(t,level,TRAIN_PHASE);
   //sbb.resetOutcomes(TEST_PHASE);
   sbb.makeEvaluationVector(t,TEST_PHASE,false);
   if (sbb.evaluationVectorSize() > 0){
      prepareOutFile(ofs,"checkpoints","cp",level,t,sbb.id(),sbb.seed(),TEST_PHASE);
      sbb.writeCheckpoint(TEST_PHASE, ofs); ofs.close();
      if (sbb.evaluationVectorSize() != max(0,sbb.testPhaseEpochs() - sbb.currentChampNumOutcomes(TEST_PHASE))){
         oss << "evaluation vector size: " << sbb.evaluationVectorSize() << " should be: " << sbb.testPhaseEpochs() - sbb.currentChampNumOutcomes(TEST_PHASE);
         die(__FILE__, __FUNCTION__, __LINE__, oss.str().c_str());
      }
      runEval(t,level,sbb.evaluationVectorSize(),TEST_PHASE,sizeTeamA,sizeTeamB, policy);
      for (int i = 1; i <= sizeTeamA; i++)
         sbb.processEvalResults(t,level,TEST_PHASE,i);
   }
   //log test timing
   if(getusage(sys2, usr2) == 0)
      die(__FILE__, __FUNCTION__, __LINE__, "cannot get rusage");
   oss << "soc::rusage test t " << t << " l " << level;
   oss << " sys1 " << sys1 << " usr1 " << usr1 << " sum1 " << (sys1 + usr1);
   oss << " sys2 " << sys2 << " usr2 " << usr2 << " sum2 " << (sys2 + usr2);
   oss << " testtime " << (sys2 + usr2) - (sys1 + usr1) << " testSec " << time(NULL) - timeTemp;
   oss << endl;
   cout << oss.str();
}
///***********************************************************************************************************************/
///*
// * Load a policy, then read features and expected actions from a file. The features
// * and result of sbb.getAction are then output for comparison with input file.
// */
//void varifyPolicy(int policy_number){
//   ofstream outFile;
//   char outputFilename[80];
//   sprintf(outputFilename, "sample.%d.policy.confirmation",policy_number);
//   if (fileExists(outputFilename))
//      remove(outputFilename);
//   outFile.open(outputFilename, ios::out);
//   if (!outFile) {
//      cerr << "Can't open eval file write: " << outputFilename << endl;
//      exit(1);
//   }
//   outFile.precision(numeric_limits< double >::digits10+1);
//   ifstream inFile;
//   char inputFilename[80];
//   sprintf( inputFilename, "sample.%d.policy",policy_number);
//   inFile.open(inputFilename, ios::in);
//   if (!inFile) {
//      oss.str("");
//      oss << "Can't open policy example file:" << inputFilename;
//      die(__FILE__, __FUNCTION__, __LINE__, oss.str().c_str());
//      oss.str("");
//   }
//   string oneline;
//   char delim = ' ';
//   vector <string> outcomeFields;
//   vector <double> features;
//   int act;
//   while (getline(inFile, oneline)){
//      outcomeFields.clear();
//      split(oneline,delim,outcomeFields);
//      for (int i = 0; i < sbb.dim(); i++)
//	 features.push_back(atof(outcomeFields[i+1].c_str()));
//      vector < set <learner*, LearnerBidLexicalCompare> > learnersRanked(sbb.numLevels(),set < learner*,LearnerBidLexicalCompare>());
//      act = sbb.getAction(features, false, learnersRanked); 
//      outFile << "featureToAct " << vecToStr(features);
//      for(set<learner*, LearnerBidLexicalCompare > :: iterator it = learnersRanked[0].begin(); it != learnersRanked[0].end();++it)
//      {
//	 outFile << " [" << (*it)->key() << "," << (*it)->esize() << "," << (*it)->refs() << "," << (*it)->numFeatures();
//	 outFile << "," << (*it)->gtime() << "," << (*it)->ancestralGtime() << "," << (*it)->id() << "," << (*it)->lastCompareFactor() << "]";
//      }
//      outFile << endl;
//      features.clear();
//   }
//
//   outFile.close();
//   exit(0);
//}
#endif
