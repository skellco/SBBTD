#ifndef SBBMOD
#define SBBMOD
#include "sbbPoint.h"
#include "sbbTeam.h"
#include "sbbLearner.h"
#include "soccer.h"

class sbbTD
{
   public:
      sbbTD(int seed = 0);
      ~sbbTD();

      /********************************************************************************************
       * Methods that set or query parameters. 
       ********************************************************************************************/

      inline int dim(){ return _dim; }
      inline void dim(int dim){ _dim = dim; }
      inline int diversityMode(){ return _diversityMode; }
      inline void diversityMode(int i){ _diversityMode = i; }
      inline bool fancyLogging() { return _fancyLogging; }
      inline void fancyLogging(bool b) { _fancyLogging = b; }
      inline int episodesPerGeneration() { return _episodesPerGeneration; }
      inline void episodesPerGeneration(int e) { _episodesPerGeneration = e; }
      inline int hostDistanceMode(){ return _distMode; }
      inline void hostDistanceMode(int i){ _distMode = i; }
      inline int hostFitnessMode(){ return _fitMode; }
      inline void hostFitnessMode(int i){ _fitMode = i; }
      inline long id() { return _id; }
      inline void id(long id) { _id = id; }
      inline double maxTrainingReward() { return _maxTrainingReward; }
      inline void  maxTrainingReward(double d) { _maxTrainingReward = d;}
      inline bool monolithic() { return _monolithic; } 
      inline int numLevels() { return _numLevels; }
      inline void numLevels(int l) { _numLevels = l;}
      inline int numProfilePoints() { return _profilePointsFIFO.size(); }
      inline int numStoreOutcomesPerHost(int phase) { return _numStoredOutcomesPerHost[phase]; }
      inline void numStoredOutcomesPerHost(int phase, long nso) { _numStoredOutcomesPerHost[phase] = nso; }
      inline int phase() { return _phase; }
      inline void phase(int p) { _phase = p; } 
      inline long seed() const { return _seed; }
      inline void seed(long seed) {
	 _seed = seed;
	 srand48(_seed);
	 //readCheckpoint would find the highest team_count and learner_count and start there
	 team_count = 1000*seed;
	 learner_count = 1000*seed;
	 point_count = 1000*seed;
      }
      inline long setupTeamCount(int s) { team_count = 1000*s; }
      inline bool splitLevel(){ return _splitLevel; }
      inline int stateDiscretizationSteps() { return _stateDiscretizationSteps; }
      inline int tCull(){ return _tCull; }
      inline int testPhaseEpochs(){ return _testPhaseEpochs; }
      inline void testPhaseEpochs(int e){ _testPhaseEpochs = e; }
      inline int validPhaseEpochs(){ return _validPhaseEpochs; }

      /********************************************************************************************
       * Methods to implement the SBB algorithm. 
       ********************************************************************************************/

      inline void activeTeam(int id){
	 for (teiterEval = _mEvaluate.begin(); teiterEval != _mEvaluate.end(); teiterEval++){
	    if ((*teiterEval)->id() == id)
	       break;
	 }
      }
      inline int activeTeamId(){ return (*teiterEval)->id();}
      inline int activeTeamLevel(){ return (*teiterEval)->level();}
      void addProfilePoint(vector < double > &state, int dim, double reward_1, double reward_2, double reward_3, int phase, int gen){
	 _profilePointsFIFO.push_back(new point(point_count, dim, gen, &state[0], point_count++, reward_1, reward_2, reward_3, phase));
	 while (_profilePointsFIFO.size() > PROFILE_SIZE){
            point *p = _profilePointsFIFO.front();
            _profilePointsFIFO.pop_front();
            delete p;
         }
      }
      void adjustIds(int);
      void checkRefCounts(const char *);
      void cleanup (long,long,bool);
      void clear();
      inline int currentChampId(){ return currentChampion->id();}
      inline int currentChampLevel(){ return currentChampion->level();}
      inline int currentChampNumOutcomes(int phase){ return currentChampion->numOutcomes(phase);}
      void debugRefs(long,long);
      void diversityMode0(long, long);
      //void diversityMode4(long, long);
      inline int evaluationVectorSize(){ return _mEvaluate.size(); }
      void finalize();
      void finalfinalize();
      void genTeams(long, long);
      void genTeams(long, team *, team **, vector < learner * > &, long);
      void genUniqueLearner(learner *, set < learner * >);
      inline int getAction(vector < double > &state, vector < learner * > &winner, vector < double > &bid1, vector < double > &bid2, bool updateActive)
      {
	 return (*teiterEval)->getAction(state, winner, bid1, bid2, updateActive);
      }
      inline int getAction(vector < double > &state, bool updateActive, vector < set <learner *, LearnerBidLexicalCompare> > &learnersRanked, long &inst, vector < long > &policyTreeTraceIds)
      {
	 return (*teiterEval)->getAction(state, updateActive, learnersRanked, inst, policyTreeTraceIds);
      }
      inline long getCurrentTeamId(){ return (*teiterEval)->id(); }
      inline void getFirstTeam(){ teiterEval = _mEvaluate.begin(); }
      inline void getLastTeam(){ teiterEval = _mEvaluate.end(); }
      inline void getLearnerPop(set <learner *> &lp){ lp = _L; }
      inline void getNextTeam(){ if (teiterEval != _mEvaluate.end()) teiterEval++; }
      void getBestTeam(long, long, int );
      void initTeams(long);
      point * initUniformPointGeneric(long, long, long, int);
      inline int lLevelSize(){ return _lLevel.size(); }
      inline int lSize(){ return _L.size(); }
      void makeEvaluationVector(int,int,bool,int playGames = 0);
      inline int mLevelSize(){ return _mLevel.size(); }
      inline int mSize(){ return _M.size(); }
      void paretoRanking(long, long, int phase = 0);
      void paretoRankingR1R2(long, long, int phase = 0);
      /* Number of actions available to the learners at the specified
       * level.
       * Define numActions(0) as number of actions in the environment,
       * else, it's the number of teams in the level below that can be
       * referenced by the specified level.
       */
      inline long numActions(long level)
      { if(level == 0) return _numActions; return _mLevel[level]->size(); }
      inline void numAtomicActions(int a) { _numActions = a; }
      void printEvaluationVector(long t);
      void printLearnerInfo(long, long);
      void printTeamInfo(long,long,int);
      void processEvalResults(int,int,int,int);
      /* Remove inactive learners. If true is passed check if team size equals _omega first. */
      void pruneLearners(bool);
      long readCheckpoint(int,istream&);
      double readRunShare();
      void recalculateLearnerRefs();
      inline void replaceLearnerPop(set <learner *> lp){ _L = lp; }
      inline void resetOutcomes(int phase){
	 set < team * , teamIdComp > :: iterator teiter;
	 for(teiter = _M.begin(); teiter != _M.end(); teiter++)
	    (*teiter)->resetOutcomes(phase);
      }
      void scaleTeamAndLearnerIds(long);
      void selTeams(long,long);
      void setOutcome(vector < double > &state, int dim, double reward_1, double reward_2, double reward_3, int phase, int gen){
	 (*teiterEval)->setOutcome(new point(point_count, dim, gen, &state[0], point_count++, reward_1, reward_2, reward_3, phase),reward_1,_numStoredOutcomesPerHost[phase]);
      }
      void setParams();
      inline long simTime() { return _simTime; }
      void splitLevelPrep();
      void stats (long, long);
      void testSymbiontUtilityDistance();
      void writeCheckpoint(int,ostream&) const;
      void writeEval(int,int,int);
      bool writeRunShare(double);

      /********************************************************************************************
       *  sbbTD member variables and data structures.
       ********************************************************************************************/

   protected:
      /* Populations at the current level begin trained. */
      set < team *, teamIdComp > _M; /* Teams */
      set < learner * > _L; /* Learners. */
      vector < team * > _mEvaluate;

      /*
       * Store teams from previous levels here. Element _mLevel[i]
       * contains the teams evolved at level i - 1. So, if evolving
       * level 1 teams, the actions available to those teams are in
       * _mLevel[1].
       *
       * Also save the point and learners at each level using the same
       * convention.
       * Hence, teams accessed by level 1 teams are at _mLevel[1], and
       * _mLevel[0] should never be accessed.
       * */

      vector < vector < team * > * > _mLevel;
      vector < vector < learner * > * > _lLevel;

      vector < double > bid1; /* First highest bid at each level, reporting only */
      vector < double > bid2; /* Second highest bid at each level, reporting only */
      team * currentChampion;
      double _compatibilityThresholdGeno;
      double _compatibilityThresholdIncrement;
      double _compatibilityThresholdPheno;
      int _dim;
      int _diversityMode;
      int _fancyLogging; /* logging, reporting */
      long _episodesPerGeneration; /* How many games should each team get to play before calculating fit and performing selection. */
      int _distMode;
      int _fitMode;
      //	void getDistinctions(set < point * > &, map < point *, vector < short > * > &);
      int _id;
      int _keepawayStateCode;
      int _knnNov;
      long learner_count;
      int _maxProgSize;
      double _maxTrainingReward;
      long _mdom; /* Number of individuals pushed out of the population by younger individuals. */
      int _Mgap; /* Team generation gap. */
      int _minOutcomesForNoveltyArchive;
      int    m_lastAction;
      bool _monolithic;
      int _Msize; /* Team population size. */
      long _numActions; /* Number of actions, actions 0, 1, ..., _numActions - 1 are assumed. */
      int _numExpectedAdditionsToArchive;
      int _numLevels; /* Number of levels to train for. */
      int _numStoredOutcomesPerHost[4];
      int _omega; /* Maximum team size. */
      ostringstream oss; /* logging, reporting */
      double _paretoEpsilonTeam;
      double _pBidAdd;
      double _pBidDelete;
      double _pBidMutate;
      double _pBidSwap;
      long _pdom; /* Number of individuals pushed out of the population by younger individuals. */
      int _phase; // 0:train 1:validation 2:test
      long point_count;
      double _pma; /* Probability of learner addition. */
      double _pmd; /* Probability of learner deletion.*/
      double _pmm; /* Probability of learner mutation. */
      double _pmn; /* Probability of learner action mutation. */
      double _pNoveltyGeno; //percentage of nov relative to fit in score
      double _pNoveltyPheno; //percentage of nov relative to fit in score
      vector < point * > _profilePoints; /* Points used to build the learner profiles. */
      deque < point * > _profilePointsFIFO;
      int _seed;
      double _sigmaShare;
      long _simTime; //simulator time
      set < point * > _solvedPoints; /* Unique points solved so far during training. */
      bool _splitLevel;
      int _stateDiscretizationSteps;
      int _tCull; /* what generation to test for restar. */
      long team_count;
      int _teamPow;
      vector < team * > :: iterator teiterEval, teiterEvalEnd;
      long _testPhaseEpochs; /* How many epochs in test phase. */
      long _validPhaseEpochs; /* How many epochs in validation phase. */
      vector < learner * > winner; /* Winner at each level, reporting only */  
};

#endif
