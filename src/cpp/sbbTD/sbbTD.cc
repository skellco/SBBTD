#include <string>  // for memcpy
#include <stdlib.h>  // for rand
#include <iterator>
#include <time.h>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include "sbbTD.h"
#include <sys/stat.h>

#define MEAN_OUT_PROP 1.0

/********************************************************************************************/
sbbTD::sbbTD(int seed)
{
   team_count = 10000*seed;
   learner_count = 10000*seed;
   point_count = 10000*seed;
   //SBB
   /* Placeholder null pointers that should never get accessed.
    *
    *   Set this way because element i is set to contain individuals
    *   evolved at level i - 1 which does not make sense for element 0.
    */
   _mLevel.push_back(0);
   _lLevel.push_back(0);

   _seed = 0; //will be set to port # later
   _id = -1;
   _simTime = 0;
}

sbbTD::~sbbTD()
{
   ;
}

/*********************************************************************************************/
void sbbTD::adjustIds(int m){
   set < learner * > :: iterator leiter, leiterEnd;
   for(leiter = _L.begin(); leiter != _L.end(); leiter++)
      (*leiter)->setId((*leiter)->id()*m);
   set < team * > :: iterator teiter, teiterEnd;
   for(teiter = _M.begin(); teiter != _M.end(); teiter++){
      (*teiter)->id((*teiter)->id()*m);

   }
}
/********************************************************************************************/
void sbbTD::checkRefCounts(const char *msg){
   int nrefs = 0;
   int sumTeamSizes = 0;
   set < learner * > :: iterator leiter, leiterEnd;
   set < team * > :: iterator teiter, teiterEnd;

   for(leiter = _L.begin(); leiter != _L.end(); leiter++)
      nrefs += (*leiter)->refs();
   for(teiter = _M.begin(); teiter != _M.end(); teiter++)
      sumTeamSizes += (*teiter)->size();
   if(sumTeamSizes != nrefs){
      cerr << "nrefs " << nrefs << " sumTeamSize " << sumTeamSizes << endl;
      die(__FILE__, __FUNCTION__, __LINE__, msg);
   }
}
/********************************************************************************************/
void sbbTD::cleanup(long t,
      long level, bool prune)
{
   /* Delete learners that are no longer referenced. */
   /* Delete cached outcomes/margins for deleted points. */

   //set < point * > :: iterator poiter, poiterend;
   set < team *, teamIdComp > :: iterator teiter, itBegin, teiterEnd;
   set < learner * > :: iterator leiter, leiterEnd;

   //vector < point * > pointvec;
   vector < learner * > learnervec;

   int size;
   int i;

   // Prune the learners, check _omega if this is not the last generation.
   if (prune == true)
     pruneLearners(false);


   // Collect learners into learnervec.

   for(leiter = _L.begin(), leiterEnd = _L.end();
	 leiter != leiterEnd; leiter++)
      learnervec.push_back(*leiter);

   // Delete learners with zero references.

   size = learnervec.size();
   for(i = 0; i < size; i++)
   {
      if(learnervec[i]->refs() == 0)
      {
	 // Zero references, delete this learner.
	 _L.erase(learnervec[i]);
	 delete learnervec[i];
      }
   }

#ifdef MYDEBUG
   int sumTeamSizes=0;
   int nrefs = 0;
   int sumNumOutcomes = 0;

   cout << "scm::cleanup ";
   //cout << _P.size() << " points, " << _L.size() << " learners, " << _M.size() << " teams";
   cout << _L.size() << " learners, " << _M.size() << " teams";
   for(teiter = _M.begin(), sumTeamSizes = sumNumOutcomes = 0; teiter != _M.end(); teiter++)
   {
      sumTeamSizes += (*teiter)->size();
      sumNumOutcomes += (*teiter)->numOutcomes(TRAIN_PHASE);
   }

   for(leiter = _L.begin(), nrefs = 0; leiter != _L.end(); leiter++)
      nrefs += (*leiter)->refs();

   cout << ", sumTeamSizes " << sumTeamSizes << ", nrefs " << nrefs << ", sumNumOutcomes " << sumNumOutcomes;
   cout << endl;

   if(sumTeamSizes != nrefs)// || sumNumOutcomes != (_Msize - _Mgap) * (_Psize - _Pgap))
      die(__FILE__, __FUNCTION__, __LINE__, "something messed up during cleanup");

#endif
}

/***********************************************************************************************************/
void sbbTD::clear()
{
   /* Free any allocated memory before exit. */

   _M.clear();
   _L.clear();

   int i, j;

   /* Teams. */

   for(i = 1; i < _mLevel.size(); i++)
   {
      for(j = 0; j < _mLevel[i]->size(); j++)
	 delete (*_mLevel[i])[j];

      delete _mLevel[i];
   }

   /* Learners. */

   for(i = 1; i < _lLevel.size(); i++)
   {
      for(j = 0; j < _lLevel[i]->size(); j++)
	 delete (*_lLevel[i])[j];

      delete _lLevel[i];
   }
}

/********************************************************************************************/
void sbbTD::debugRefs(long t,
      long level)
{
   set < team *, teamIdComp > :: iterator teiter, itBegin, teiterEnd;
   set < learner * > :: iterator leiter, leiterEnd;

   int nrefs = 0;
   int sumTeamSizes = 0;
   int sumNumOutcomes = 0;

   cout << "scm::debugRefs ";
   //cout << _P.size() << " points, " << _L.size() << " learners, " << _M.size() << " teams";
   cout << _L.size() << " learners, " << _M.size() << " teams";
   for(teiter = _M.begin(), sumTeamSizes = sumNumOutcomes = 0; teiter != _M.end(); teiter++)
   {
      sumTeamSizes += (*teiter)->size();
      sumNumOutcomes += (*teiter)->numOutcomes(TRAIN_PHASE);
   }

   for(leiter = _L.begin(), nrefs = 0; leiter != _L.end(); leiter++)
      nrefs += (*leiter)->refs();

   cout << ", sumTeamSizes " << sumTeamSizes << ", nrefs " << nrefs << ", sumNumOutcomes " << sumNumOutcomes;
   cout << endl;

   if(sumTeamSizes != nrefs)// || sumNumOutcomes != (_Msize - _Mgap) * (_Psize - _Pgap))
      die(__FILE__, __FUNCTION__, __LINE__, "something messed up during cleanup");

   //#endif
}

/********************************************************************************************
 * No diversity.
 */
void sbbTD::diversityMode0(long t, long level){
   set < team *, teamIdComp > :: iterator teiter, teiterend;

   for(teiter = _M.begin(); teiter != _M.end(); teiter++){
      (*teiter)->score((*teiter)->getMeanOutcome(_fitMode,TRAIN_PHASE,MEAN_OUT_PROP));
      (*teiter)->fit((*teiter)->getMeanOutcome(_fitMode,TRAIN_PHASE,MEAN_OUT_PROP));
   }
   //fit normalized wrt population
   //vector < team * > teams;
   //for(teiter = _M.begin(); teiter != _M.end(); teiter++)
   //  teams.push_back(*teiter);
   //     
   //
   //double meanOutMax = 0;
   //double meanOutMin = HUGE_VAL;

   //for(int i = 0; i < teams.size(); i++){
   //   double meanOut = teams[i]->getMeanOutcome(TRAIN_REWARD,TRAIN_PHASE);
   //   if (meanOut < meanOutMin)
   //      meanOutMin = meanOut;
   //   if (meanOut > meanOutMax)
   //      meanOutMax = meanOut;
   //}
   ////set normalized fit (score just equal to fit)
   //for(int i = 0; i < teams.size(); i++){
   //   teams[i]->fit((teams[i]->getMeanOutcome(TRAIN_REWARD,TRAIN_PHASE)-meanOutMin)/(meanOutMax-meanOutMin));
   //   teams[i]->score((teams[i]->getMeanOutcome(TRAIN_REWARD,TRAIN_PHASE)-meanOutMin)/(meanOutMax-meanOutMin));
   //}
}

/*********************************************************************************************
 * Helper struct for distance comparisons.
 */

struct distanceInstance {
   double distance;
   bool fromArchive;
   distanceInstance(double d,bool a):distance(d),fromArchive(a) {}
} ;

bool compareByDistance(const distanceInstance &a, const distanceInstance &b)
{
   return a.distance < b.distance;
}

/********************************************************************************************/
void sbbTD::finalize(){
   //set < point * > :: iterator poiter;
   set < team *, teamIdComp > :: iterator teiter;

   // Purge outcomes and margins since they won't be used.
   /*
      for(teiter = _M.begin(); teiter != _M.end(); teiter++)
      for(poiter = _P.begin(); poiter != _P.end(); poiter++)
      {
      (*teiter)->deleteOutcome(*poiter);
      (*teiter)->deleteMargin(*poiter);
      }
    */

   _mLevel.push_back(new vector < team * >);
   (_mLevel.back())->insert((_mLevel.back())->begin(), _M.begin(), _M.end());
   _M.clear();

   _lLevel.push_back(new vector < learner * > );
   (_lLevel.back())->insert((_lLevel.back())->begin(), _L.begin(), _L.end());
   _L.clear();

   /* Hence _M and _L are purged but we saved the pointers to the
      points, teams, and learners. */ 

   _mdom = _pdom = 0;

   for (vector<point *>::iterator it = _profilePoints.begin() ; it != _profilePoints.end(); it++)
      delete *it;

   while (_profilePointsFIFO.size() != 0){
      point *p = _profilePointsFIFO.front();
      _profilePointsFIFO.pop_front();
      delete p;
   }
}
/***********************************************************************************************************/

void sbbTD::finalfinalize(){
   /* Free any allocated memory before exit. */

   int i, j;

   /* Teams. */

   for(i = 1; i < _mLevel.size(); i++)
   {
      for(j = 0; j < _mLevel[i]->size(); j++)
	 delete (*_mLevel[i])[j];

      delete _mLevel[i];
   }

   /* Learners. */

   for(i = 1; i < _lLevel.size(); i++)
   {
      for(j = 0; j < _lLevel[i]->size(); j++)
	 delete (*_lLevel[i])[j];

      delete _lLevel[i];
   }
}

/********************************************************************************************/
void sbbTD::genTeams(long t,
      long level)
{
   vector < team * > parent;
   int psize;

   team *pm;
   team *cm;

   vector < learner * > lpop;

   parent.insert(parent.begin(), _M.begin(), _M.end());
   psize = parent.size();
   lpop.insert(lpop.begin(), _L.begin(), _L.end());
   //std::pair<std::set<team *, teamIdComp>::iterator,bool> ret;
   while(_M.size() < _Msize)
   {
      pm = parent[(int) (drand48() * psize)];
      genTeams(t, pm, &cm, lpop, level);
      _M.insert(cm);
   }
}

/********************************************************************************************/
void sbbTD::genTeams(long t,
      team *pm,
      team **cm,
      vector < learner * > &lpop,
      long level)
{
   set < learner * > plearners;
   set < learner * > clearners;

   set < learner * > :: iterator leiter;

   vector < learner * > learnervec;

   vector <int> actions_used;
   int i;
   int nsearch = 50;

   double b;

   learner *lr;

   bool changedL;
   bool changedM;

   int lsize;

   lsize = lpop.size();

   pm->members(&plearners);
   *cm = new team(level, _mLevel[level], t, team_count++);
   for(leiter = plearners.begin(); leiter != plearners.end(); leiter++)
      (*cm)->addLearner(*leiter);
   learnervec.insert(learnervec.begin(), plearners.begin(), plearners.end());

   // Remove learners.

   for(b = 1.0; drand48() < b && (*cm)->size() > 2; b = b * _pmd)
   {
      do
      {
	 i = (int) (drand48() * learnervec.size());
      }
      while(learnervec[i] == 0);
      (*cm)->removeLearner(learnervec[i]);
      learnervec[i] = 0;
   }

   // Add learners.

   if (_monolithic == true){
      clearners.clear();
      (*cm)->members(&clearners);
      for(leiter = clearners.begin(); leiter != clearners.end(); leiter++)
	 actions_used.push_back((*leiter)->action());
   }

   for(b = 1.0; drand48() < b && (*cm)->size() < _omega && (*cm)->size() < lsize; b = b * _pma)
   {
      if (_monolithic){
	 bool teamAdded = false;
	 int t;
	 int numTries = lpop.size()*2;
	 while (teamAdded == false && t <= numTries){
	    i = (int) (drand48() * lsize);
	    if (find(actions_used.begin(), actions_used.end(), lpop[i]->action()) == actions_used.end()){
	       teamAdded = (*cm)->addLearner(lpop[i]);
	    }
	    if (teamAdded == true)
	       actions_used.push_back(lpop[i]->action());
	    t++;
	 }
      }
      else{
	 do
	 {
	    i = (int) (drand48() * lsize);
	 }
	 while((*cm)->addLearner(lpop[i]) == false);
      }
   }

   // Mutate learners.

   clearners.clear();
   (*cm)->members(&clearners);

   changedM = false;

   do
   {
      for(leiter = clearners.begin(); leiter != clearners.end(); leiter++)
      {
	 if(drand48() < _pmm)
	 {
	    changedM = true;

	    (*cm)->removeLearner(*leiter);

	    lr = new learner(t, **leiter, learner_count++);

	    do
	    {
	       changedL = lr->muBid(_pBidDelete, _pBidAdd, _pBidSwap, _pBidMutate, _maxProgSize);
	    }
	    while(changedL == false);

	    genUniqueLearner(lr, _L); /* Make sure bid unique. */

	    if(drand48() < _pmn && _monolithic == false){
	       if (_splitLevel == true){
		  //don't mutate action in split level
		  //lr->muAction(((long) (drand48() * numActions(0))));  
		  // //splitlevel transfer: make sure new actions not pointing to lower level are marked atomic
		  //if (lr->action() == 0 && drand48() < 0.5 || lr->action() > 0)
		  //   lr->atomic(true);
	       }
	       else
		  lr->muAction(((long) (drand48() * numActions(level))));
	    }

	    (*cm)->addLearner(lr);
	    _L.insert(lr);
	 }
      }
   }
   while(changedM == false);

   /* Increment references. */

   clearners.clear();
   (*cm)->members(&clearners);

   for(leiter = clearners.begin(); leiter != clearners.end(); leiter++)
      (*leiter)->refInc();

   (*cm)->parentScore(pm->fit());
   (*cm)->addAncestor(pm->id());

   //copy all parents ancestors
   vector <long> parentAncestors;
   pm->getAllAncestors(parentAncestors);
   for (int i = 0; i < parentAncestors.size(); i++)
      (*cm)->addAncestor(parentAncestors[i]);
}

/********************************************************************************************/
// Keep altering 'lr' as long as its bid profile is the same as the bid
//   profile of one of the learners in 'learners'. 
void sbbTD::genUniqueLearner(learner *lr,
      set < learner * > learners)
{
   vector < double > profile;
   set < learner * > :: iterator leiter;

   vector < double > state;

   int i;

   bool stop = false;

   bool changedL;

   double *REG = (double *) alloca(REGISTERS * sizeof(double));

#ifdef MYDEBUG
   for(i = 0, leiter = learners.begin(); leiter != learners.end(); leiter++, i++)
   {
      (*leiter)->getProfile(profile);
      cout << "scm::genUniqueLearner " << lr->id() << " " << i << " " << (*leiter)->id() << " profile" << vecToStr(profile) << endl;
   }
#endif

   while(stop == false)
   {
      profile.clear();

      // Create new profile.

      for(i = 0; i < _profilePointsFIFO.size(); i++)
      {
	 _profilePointsFIFO[i]->state(state);
	 profile.push_back(lr->bid(&state[0], REG));
      }

#ifdef MYDEBUG
      cout << "scm::genUniqueLearner " << lr->id() << " profile" << vecToStr(profile);
#endif

      if(profile.size() != _profilePointsFIFO.size())
	 die(__FILE__, __FUNCTION__, __LINE__, "bad profile size");

      for(leiter = learners.begin(); leiter != learners.end(); leiter++)
	 if((*leiter)->isProfileEqualTo(profile))
	    break;

      if(leiter == learners.end())
      {
	 //Not a duplicate bidder.

#ifdef MYDEBUG
	 cout << " is unique" << endl;
#endif

	 stop = true;
      }
      else
      {
	 // Duplicate bidder.

#ifdef MYDEBUG
	 cout << " is a duplicate of " << (*leiter)->id() << endl;
#endif

	 do
	 {
	    changedL = lr->muBid(_pBidDelete, _pBidAdd, _pBidSwap, _pBidMutate, _maxProgSize);
	 }
	 while(changedL == false);
      }
   }

   lr->setProfile(profile);
}


/********************************************************************************************/
void sbbTD::getBestTeam(long t, long level,int phase){
   set < team * , teamIdComp> :: iterator teiter, teiterend;
   double maxMeanOut = -HUGE_VAL;
   double meanOut = 0;

   oss.str("");
   currentChampion = (*_M.begin());
   for(teiter = _M.begin(); teiter != _M.end(); teiter++){
      if ((*teiter)->numOutcomes(phase) > _episodesPerGeneration && (*teiter)->numOutcomes(TEST_PHASE) == 0){ //only consider hosts that are older than 1 generation and not yet tested
	 meanOut = (*teiter)->getMeanOutcome(TRAIN_REWARD,phase,MEAN_OUT_PROP);
	 oss << "sbb::getBestTeamAll phase " << phase << " l " << level << " t " << t << " tm " << (*teiter)->id();
	 oss << " score " << meanOut << " numOutcome " << (*teiter)->numOutcomes(phase) << endl;
	 if (meanOut > maxMeanOut){
	    currentChampion = (*teiter);
	    maxMeanOut = meanOut;
	 }
      }
   }
   oss << "sbb::getBestTeam l " << level << " t " << t << " " << currentChampion->toString("maxteam");
   cout << oss.str();
   oss.str("");
   oss << "sbb::getBestTeam best l " << level << " t " << t << " meanOutcome " <<  maxMeanOut;
   oss << " age " << t - currentChampion->gtime();
   oss << " " << *currentChampion << endl;
   cout << oss.str();
   oss.str("");

   //cout.precision(numeric_limits< double >::digits10+1);
   //set < team * , teamIdComp> :: iterator teiter, teiterend;
   //paretoRanking(t,level,VALIDATION_PHASE);
   //set <team *, teamScoreLexicalCompare> teams;
   //for(teiter = _M.begin(); teiter != _M.end(); teiter++){
   //   //(*teiter)->score((*teiter)->getMeanOutcome(TEST_REWARD,phase));
   //   teams.insert(*teiter);
   //}

   //currentChampion = *(teams.begin());

   //cout << "getBestTeamRanking phase " << phase << " l " << level << " t " << t;
   //set < long > policyFeatures;
   //int numLearnersInPolicy;
   //for(set<team*, teamScoreLexicalCompare > :: iterator it = teams.begin(); it != teams.end();++it){
   //   numLearnersInPolicy = (*it)->policyFeatures(policyFeatures);
   //   cout << std::fixed << " [" << (*it)->score();
   //   cout << "," << (*it)->asize() << "," << numLearnersInPolicy << "," << policyFeatures.size() << "," << (*it)->gtime();
   //   cout << "," << (*it)->id() << "," << (*it)->lastCompareFactor() << "]";
   //   policyFeatures.clear();
   //}
   //cout << endl;

   //cout << std::fixed << "sbb::getBestTeam best l " << level << " t " << t << " score " <<  currentChampion->score();;
   //cout << " age " << t - currentChampion->gtime();
   //cout << " " << *currentChampion << endl;
}

/********************************************************************************************/
void sbbTD::initTeams(long level)
{
   /* Bidding behaviour. */
   while(_profilePoints.size() < PROFILE_SIZE_INI)
      _profilePoints.push_back(initUniformPointGeneric(-3, _dim, point_count++, VALIDATION_PHASE));
   while(_profilePointsFIFO.size() < PROFILE_SIZE_INI)
      _profilePointsFIFO.push_back(initUniformPointGeneric(-3, _dim, point_count++, VALIDATION_PHASE));

   vector <long> actions_used;
   long a1, a2;
   team *m;
   learner *l;

   // Number of teams to initialize.
   int keep = _Msize - _Mgap;

   int tc;

   int i, j, k;

   set < team * > :: iterator teiter;

   vector < learner * > learnervec;

   int size;

   set < learner * > lused;
   set < learner * > :: iterator leiter;


   int tsize = 10;

   for(tc = 0; tc < keep; tc++)
   {
      // Get two different actions.
      if (_splitLevel == true)
	 a1 = 0;//(long) (drand48() * numActions(0));
      else
	 a1 = (long) (drand48() * numActions(level));

      if (_splitLevel == true)
	 a2 = 0;
      else{
	 do
	 {
	    //if (_splitLevel == true)
	    //   a2 = (long) (drand48() * numActions(0));
	    //else
	    a2 = (long) (drand48() * numActions(level));
	 } while (a1 == a2);
      }


      // Create a new team containing two learners.

      m = new team(level, _mLevel[level], 1, team_count++);

      l = new learner(1, a1, _maxProgSize, _dim, learner_count++);
      genUniqueLearner(l, _L);  //Make sure bid unique.
      //if (level == 0 || (_splitLevel == true && ((a1 == 0 && drand48() < 0.5) || level > 0 && a1 > 0)))
      if (level == 0 || _splitLevel == true)
	 l->atomic(true);
      m->addLearner(l);
      l->refInc();
      _L.insert(l);
      l = new learner(1, a2, _maxProgSize, _dim, learner_count++);
      genUniqueLearner(l, _L); // Make sure bid unique.
      //if (level == 0 || (_splitLevel == true && ((a2 == 0 && drand48() < 0.5) || level > 0 && a2 > 0)))
      if (level == 0 || _splitLevel == true)
	 l->atomic(true);
      m->addLearner(l);
      l->refInc();
      _L.insert(l);
      _M.insert(m);
#ifdef MYDEBUG
      cout << "scm::initTeams added " << *m << endl;
#endif
   }

   // Mix the learners up.

   learnervec.insert(learnervec.begin(), _L.begin(), _L.end());

   for(teiter = _M.begin(); teiter != _M.end(); teiter++)
   {
      // Get the learners in the team.
      lused.clear();
      (*teiter)->members(&lused);
      actions_used.clear();
      for(leiter = lused.begin(); leiter != lused.end(); leiter++)
	 actions_used.push_back((*leiter)->action());
      // Get the final team size, make sure not too big.
      size = (int) (drand48() * (5 + 1));  
      //size = (int) (drand48() * (_omega + 1));

      if(size > learnervec.size() - 2)
	 size = learnervec.size() - 2;

      if  (_monolithic == true) //(_monolithic == true && size > _numActions)
	 size = _omega;

      // Build the team up.
      int numTries = learnervec.size()*2;
      int t = 0;
      while((*teiter)->size() < size && t <= numTries)
      {
	 // Get first learner, make sure not in the team.

	 i = (int) (drand48() * learnervec.size());

	 if (_monolithic == true){

	    while(t <= (numTries/2) && (lused.find(learnervec[i]) != lused.end() || find(actions_used.begin(), actions_used.end(), learnervec[i]->action()) != actions_used.end())){
	       i = (i + 1) % learnervec.size();
	       t++;
	    }
	 }

	 else{
	    while(lused.find(learnervec[i]) != lused.end())
	       i = (i + 1) % learnervec.size();
	 }

	 for(k = 0; k < tsize; k++)
	 {
	    // Get another learner, make sure not in the team.

	    j = (int) (drand48() * learnervec.size());

	    if (_monolithic == true){
	       while(t <= numTries && (lused.find(learnervec[j]) != lused.end() || find(actions_used.begin(), actions_used.end(), learnervec[j]->action()) != actions_used.end())){
		  j = (j + 1) % learnervec.size();
		  t++;
	       }
	    }
	    else{
	       while(lused.find(learnervec[j]) != lused.end())
		  j = (j + 1) % learnervec.size();
	    }

	    // Pick second learner if it has fewer refs.

	    if (_monolithic == false){
	       if(learnervec[j]->refs() < learnervec[i]->refs() && find(actions_used.begin(), actions_used.end(), learnervec[j]->action()) == actions_used.end())
		  i = j;
	    }
	    else {
	       if(learnervec[j]->refs() < learnervec[i]->refs())
		  i = j;
	    }
	 }

	 if (_monolithic == true){
	    if (find(actions_used.begin(), actions_used.end(), learnervec[i]->action()) == actions_used.end()){
	       (*teiter)->addLearner(learnervec[i]);
	       actions_used.push_back(learnervec[i]->action());
	       lused.insert(learnervec[i]);
	       learnervec[i]->refInc();
	    }
	 }
	 else{
	    (*teiter)->addLearner(learnervec[i]);
	    actions_used.push_back(learnervec[i]->action());
	    lused.insert(learnervec[i]);
	    learnervec[i]->refInc();
	 }
      }
   }
}

/*******************************************************************************************/
point * sbbTD::initUniformPointGeneric(long gtime, long dim, long label, int phase)
{
   vector < double > state;

   for (int i = 0; i < dim; i++)
      state.push_back(drand48()*360);

   return new point(label, dim, gtime, &state[0], label,-1.0,-1.0,-1.0,phase);
}

/***********************************************************************************************************/

// The evaluation vector, _mEvaluate, stores one team pointer for each evaluation that needs to take place at 
// this point in time. 
//
// If a team needs to be evaluated 10 times, _mEvaluate will store 10 pointers to that team. The evaluation
// routine can then loop through _mEvaluate, running each team (pointer) on the task.
// Which teams are included in the evaluation, and how many times, is dictated by their stored outcomes and
// the  phase (training, validation, test, or replay).
//
// When sbbTD is part of a "satelite" agent that only exists for a single evaluation round, as is the 
// case in some domains such as Robocup, no outcome history is present and thus tmpNumOutcomes is used 
// instead. 

void sbbTD::makeEvaluationVector(int t, int phase, bool useTmpNumOutcomes, int playGames){
   set < team * , teamIdComp > :: iterator teiter;
   _mEvaluate.clear();
   int numOuts = 0;
   int fill = 0;
   switch (phase){
      case TRAIN_PHASE: 
	 for(teiter = _M.begin(); teiter != _M.end(); teiter++){
	    if (useTmpNumOutcomes==false)
	       numOuts = (*teiter)->numOutcomes(phase);
	    else
	       numOuts = (*teiter)->tmpNumOutcomes();
	    if (numOuts < _numStoredOutcomesPerHost[phase]){
	       fill = min((int)(_numStoredOutcomesPerHost[phase] - numOuts),(int)_episodesPerGeneration);
	       for (int i = 0; i < fill; i++)
		  _mEvaluate.push_back(*teiter);
	    }
	    //else if((*teiter)->getMeanOutcome(TRAIN_REWARD,TRAIN_PHASE) < _maxTrainingReward)
	    //   _mEvaluate.push_back(*teiter);
	    //for (int i = 0; i < _episodesPerGeneration; i++)
	    //  _mEvaluate.push_back(*teiter);
	 }
	 break;
      case VALIDATION_PHASE: 
	 for(teiter = _M.begin(); teiter != _M.end(); teiter++){
	    if (useTmpNumOutcomes==false)
	       numOuts = (*teiter)->numOutcomes(phase);
	    else
	       numOuts = (*teiter)->tmpNumOutcomes();
	    for (int i = 0; i < (_validPhaseEpochs - numOuts); i++)
	       _mEvaluate.push_back(*teiter);
	 }
	 break;
      case TEST_PHASE: 
	 //this happens in "satelite" agents
	 if (_M.size() == 1) 
	    currentChampion = *(_M.begin());
	 if (useTmpNumOutcomes==false)
	    numOuts = currentChampion->numOutcomes(phase);
	 else
	    numOuts = currentChampion->tmpNumOutcomes();
	 for (int i = 0; i < (_testPhaseEpochs - numOuts); i++)
	    _mEvaluate.push_back(currentChampion);
	 break;
      case PLAY_PHASE: 
	 //this happens in "satelite" agents
	 if (_M.size() == 1){
	    for (int i = 0; i < playGames; i++){
	       _mEvaluate.push_back(*(_M.begin()));
	    }
	 }
	 else {
	    for(teiter = _M.begin(); teiter != _M.end(); teiter++){
	       for (int i = 0; i < playGames; i++){
		  _mEvaluate.push_back(*teiter);
	       }
	    }
	 }
	 break;
      default:
	 ;
	 break;
   }
}

/********************************************************************************************
 * Assign each team a score based on multi-objective Pareto ranking with nov 
 * and performance as objectives
 */
void sbbTD::paretoRanking(long t, long level, int phase){
   vector < team * > teams;
   set < team *, teamIdComp > :: iterator teiter, teiterend;

   for(teiter = _M.begin(); teiter != _M.end(); teiter++){
      teams.push_back(*teiter);
      (*teiter)->domBy(0);
      (*teiter)->domOf(0);
      (*teiter)->clearDistances();
      (*teiter)->fit((*teiter)->getMeanOutcome(_fitMode,phase,MEAN_OUT_PROP));
   }

   double d0,d1;
   //measure distances w.r.t rest of population
   for(int i = 0; i < teams.size(); i++){
      for(int j = 0; j < teams.size(); j++){
	 if (j > i){//not the same team and haven't considered this pair yet
	    d0 = teams[i]->symbiontUtilityDistance(teams[j],_omega);
	    d1 = teams[i]->ncdBehaviouralDistance(teams[j], phase);
	    teams[i]->addDistance(0,d0);
	    teams[i]->addDistance(1,d1);
	    teams[j]->addDistance(0,d0);
	    teams[j]->addDistance(1,d1);
	 }
      }
   }

   /* Pareto Scoring */
   vector < team * > dominatedTeams;
   vector < team * > nonDominatedTeams;
   vector < team * > :: iterator itA;
   vector < team * > :: iterator itB;

   dominatedTeams.clear();
   nonDominatedTeams.clear();
   double fitA, fitB, novA, novB;
   for(itA = teams.begin(); itA != teams.end(); itA++){
      novA = (*itA)->nov(_distMode,_knnNov); 
      for(itB = teams.begin(); itB != teams.end(); itB++){
	 novB = (*itB)->nov(_distMode,_knnNov);
	 //check if itB dominates itA
	 if ((((*itB)->fit()  > (*itA)->fit()  && isEqual((*itA)->fit(), (*itB)->fit(), _paretoEpsilonTeam) == false) || (novB > novA && isEqual(novA, novB, _paretoEpsilonTeam) == false)) &&
	       ((*itB)->fit() >= (*itA)->fit() && novB >= novA))
	 {
	    (*itA)->domBy((*itA)->domBy()+1);
	    (*itB)->domOf((*itB)->domOf()+1);
	    if(find(dominatedTeams.begin(), dominatedTeams.end(), *itA) == dominatedTeams.end())
	       dominatedTeams.push_back(*itA);
	 }
      }
      if ((*itA)->domBy() < 1)
	 nonDominatedTeams.push_back(*itA);
   }
   for(int i = 0; i < teams.size(); i++){
      teams[i]->score(1-((double)teams[i]->domBy()/teams.size()));
      //teams[i]->score((double)teams[i]->domOf()/teams.size());
   }

   //Novelty reporting
   oss.str("");
   oss.precision(numeric_limits< double >::digits10+1);
   cout.precision(numeric_limits< double >::digits10+1);
   oss << std::fixed << "sbb::paretoRanking t " << t << " l " << level << " _fitMode " << _fitMode << " _distMode " << _distMode << " numFront " << nonDominatedTeams.size() << " numDom " << dominatedTeams.size() << " [id,fit,nov_0,nov_1,score] dominated ";
   for (int i = 0; i < dominatedTeams.size(); i++)
      oss << std::fixed << "[" << dominatedTeams[i]->id() << "," << dominatedTeams[i]->fit() << "," << dominatedTeams[i]->nov(0,_knnNov) << "," << dominatedTeams[i]->nov(1,_knnNov) << "," << dominatedTeams[i]->score() << "] ";
   oss << " nonDominated ";
   for (int i = 0; i < nonDominatedTeams.size(); i++)
      oss << std::fixed << "[" << nonDominatedTeams[i]->id() << "," << nonDominatedTeams[i]->fit() << "," << nonDominatedTeams[i]->nov(0,_knnNov) << "," << nonDominatedTeams[i]->nov(1,_knnNov) << "," << nonDominatedTeams[i]->score() << "] ";
   oss << endl;
   cout << oss.str();
   oss.str("");
   oss.precision(3);
   cout.precision(3);
}

/********************************************************************************************
 * Assign each team a score based on multi-objective Pareto ranking with reward 1 and 2
 * as objectives.
 */
void sbbTD::paretoRankingR1R2(long t, long level, int phase){
   vector < team * > teams;
   set < team *, teamIdComp > :: iterator teiter, teiterend;

   for(teiter = _M.begin(); teiter != _M.end(); teiter++){
      teams.push_back(*teiter);
      (*teiter)->domBy(0);
      (*teiter)->domOf(0);
      (*teiter)->clearDistances();
      (*teiter)->fit((*teiter)->getMeanOutcome(1,phase,MEAN_OUT_PROP));
   }

   double d0,d1;
   //measure distances w.r.t rest of population
   for(int i = 0; i < teams.size(); i++){
      for(int j = 0; j < teams.size(); j++){
	 if (j > i){//not the same team and haven't considered this pair yet
	    d0 = teams[i]->symbiontUtilityDistance(teams[j],_omega);
	    d1 = teams[i]->ncdBehaviouralDistance(teams[j], phase);
	    teams[i]->addDistance(0,d0);
	    teams[i]->addDistance(1,d1);
	    teams[j]->addDistance(0,d0);
	    teams[j]->addDistance(1,d1);
	 }
      }
   }

   /* Pareto Scoring */
   vector < team * > dominatedTeams;
   vector < team * > nonDominatedTeams;
   vector < team * > :: iterator itA;
   vector < team * > :: iterator itB;

   dominatedTeams.clear();
   nonDominatedTeams.clear();
   double fit2A, fit2B;
   for(itA = teams.begin(); itA != teams.end(); itA++){
      fit2A = (*itA)->getMeanOutcome(2,phase,MEAN_OUT_PROP); 
      for(itB = teams.begin(); itB != teams.end(); itB++){
	 fit2B = (*itB)->getMeanOutcome(2,phase,MEAN_OUT_PROP);
	 //check if itB dominates itA
	 if ((((*itB)->fit()  > (*itA)->fit()  && isEqual((*itA)->fit(), (*itB)->fit(), _paretoEpsilonTeam) == false) || (fit2B > fit2A && isEqual(fit2A, fit2B, _paretoEpsilonTeam) == false)) &&
	       ((*itB)->fit() >= (*itA)->fit() && fit2B >= fit2A))
	 {
	    (*itA)->domBy((*itA)->domBy()+1);
	    (*itB)->domOf((*itB)->domOf()+1);
	    if(find(dominatedTeams.begin(), dominatedTeams.end(), *itA) == dominatedTeams.end())
	       dominatedTeams.push_back(*itA);
	 }
      }
      if ((*itA)->domBy() < 1)
	 nonDominatedTeams.push_back(*itA);
   }
   for(int i = 0; i < teams.size(); i++){
      teams[i]->score(1-((double)teams[i]->domBy()/teams.size()));
      //teams[i]->score((double)teams[i]->domOf()/teams.size());
   }

   //Novelty reporting
   oss.str("");
   oss.precision(numeric_limits< double >::digits10+1);
   cout.precision(numeric_limits< double >::digits10+1);
   oss << std::fixed << "sbb::paretoRanking t " << t << " l " << level << " _fitMode " << _fitMode << " _distMode " << _distMode << " numFront " << nonDominatedTeams.size() << " numDom " << dominatedTeams.size() << " [id,fit,nov_0,nov_1,score] dominated ";
   for (int i = 0; i < dominatedTeams.size(); i++)
      oss << std::fixed << "[" << dominatedTeams[i]->id() << "," << dominatedTeams[i]->fit() << "," << dominatedTeams[i]->nov(0,_knnNov) << "," << dominatedTeams[i]->nov(1,_knnNov) << "," << dominatedTeams[i]->score() << "] ";
   oss << " nonDominated ";
   for (int i = 0; i < nonDominatedTeams.size(); i++)
      oss << std::fixed << "[" << nonDominatedTeams[i]->id() << "," << nonDominatedTeams[i]->fit() << "," << nonDominatedTeams[i]->nov(0,_knnNov) << "," << nonDominatedTeams[i]->nov(1,_knnNov) << "," << nonDominatedTeams[i]->score() << "] ";
   oss << endl;
   cout << oss.str();
   oss.str("");
   oss.precision(3);
   cout.precision(3);
}
/**********************************************************************************************************/
void sbbTD::printEvaluationVector(long t){
   vector < team * > :: iterator teiter;
   cout << "printEvaluationVector t " << t << "-----------------------------------------" << endl;
   for(teiter = _mEvaluate.begin(); teiter != _mEvaluate.end(); teiter++)
      cout << (*teiter)->id() << endl;
}

/**********************************************************************************************************/
void sbbTD::printLearnerInfo(long t, long level){
   set < learner *> :: iterator leiter, leiterEnd;
   cout << "printLearnerInfo start" << endl;
   for(leiter = _L.begin(), leiterEnd = _L.end(); leiter != leiterEnd; leiter++)
      cout << (*leiter)->checkpoint() << endl;
   cout << "printLearnerInfo end" << endl;
}
/**********************************************************************************************************/
void sbbTD::printTeamInfo(long t, long level, int phase){
   set < team *, teamIdComp > :: iterator teiter, teiterend;
   ostringstream tmposs;
   map < point *, double, pointLexicalLessThan > allOutcomes;
   map < point *, double > :: iterator myoiter;
   vector <int> behaviourSequence;
   oss.str("");
   for(teiter = _M.begin(); teiter != _M.end(); teiter++){
      //cout << (*teiter)->toString("PTEAM") << endl;
      tmposs << "teamsToString t " << t << " runningLevel " << level << " ";
      oss << "-------------------------------------------------------------------" << endl;
      oss << (*teiter)->toString(tmposs.str());
      tmposs.str("");
      oss << *(*teiter) << endl;
      oss << "tminfo t " << t << " lev " << level << " id " << (*teiter)->id() << " gtime " << (*teiter)->gtime();
      oss << " size " << (*teiter)->size();
      oss << " asize " << (*teiter)->asize();
      oss << " age " << t - (*teiter)->gtime() << " numOut " << (*teiter)->numOutcomes(phase);
      oss << " meanReward_1 " << (*teiter)->getMeanOutcome(1,phase,MEAN_OUT_PROP);
      oss << " meanReward_2 " << (*teiter)->getMeanOutcome(2,phase,MEAN_OUT_PROP);
      oss << " meanReward_3 " << (*teiter)->getMeanOutcome(3,phase,MEAN_OUT_PROP);
      oss << " fit " << (*teiter)->fit();
      oss << " nov_0 " << (*teiter)->nov(0,_knnNov);
      oss << " nov_1 " << (*teiter)->nov(1,_knnNov);
      oss << " score " << (*teiter)->score();
      oss << " ePolicyInstructions " << (*teiter)->policyInstructions(true);
      oss << " policyInstructions " << (*teiter)->policyInstructions(false);
      vector <long> policyHostIds;
      (*teiter)->policyHosts(policyHostIds, 0, true);
      oss << " policyHosts " << vecToStr(policyHostIds);
      oss << " members ";
      set < learner * > mem;
      (*teiter)->members(&mem);
      set < learner * > :: iterator leiter;
      for(leiter = mem.begin(); leiter != mem.end(); leiter++)
	 oss << " |" << (*leiter)->id() << ":" << (*leiter)->action() << "|";
      oss << " amembers ";
      mem.clear();
      (*teiter)->activeMembers(&mem);
      for(leiter = mem.begin(); leiter != mem.end(); leiter++)
	 oss << " |" << (*leiter)->id() << ":" << (*leiter)->action() << ":" << (*leiter)->refs() << "|";

      oss << " childUp ";
      if ((*teiter)->score() > (*teiter)->parentScore())
	 oss << "1";
      else
	 oss << "0";
      vector <long> ancestors;
      (*teiter)->getAllAncestors(ancestors);
      oss << " ancestors" << vecToStr(ancestors);
      (*teiter)->outcomes(allOutcomes,phase);
      oss << " allOutcomes";
      for(myoiter = allOutcomes.begin(); myoiter != allOutcomes.end(); myoiter++)
	 oss << " [" << (myoiter->second) << ":" << (myoiter->first)->somedouble(1) << "," << (myoiter->first)->somedouble(2) << "," << (myoiter->first)->somedouble(3) << "]";
      oss << " behaviourProfile ";
      behaviourSequence.clear();
      (*teiter)->getBehaviourSequence(behaviourSequence,phase);
      oss << vecToStrNoSpace(behaviourSequence);
      //oss << endl;
      set < long > features;
      (*teiter)->policyFeatures(features, true);
      oss << "policyFeatures uniq " << features.size() << " feat ";
      set <long >::iterator feiter;
      for(feiter = features.begin(); feiter!=features.end();feiter++)
	 oss << " " << (*feiter); 
      oss << endl;
   }
   oss << "-------------------------------------------------------------------" << endl;
   cout << oss.str();
   oss.str("");
}

/***********************************************************************************************************/
void sbbTD::processEvalResults(int t, int level, int phase, int keeper){
   set < team *, teamIdComp > :: iterator teiter, teiterEnd;
   vector < double > testOuts;
   ifstream inFile;
   char inputFilename[80];

   oss.str("");
   oss << "evals/eval." << phase << "." << keeper << "." << _seed << ".rslt";
   sprintf( inputFilename, oss.str().c_str());

   oss.str("");
   oss << "Can't open eval file read: " << inputFilename;
   inFile.open(inputFilename, ios::in);
   if (!inFile) {
      die(__FILE__, __FUNCTION__, __LINE__, oss.str().c_str());
   }
   oss.str("");

   string oneline;
   char delim = ':';
   int id;
   double somedouble_1;
   double somedouble_2;
   double somedouble_3;
   vector < string > outcomeFields;
   vector < string > activeMemberIdsString;
   vector < long > activeMemberIds;
   vector < double > state;
   vector < string > stateString;
   oss.str();

   while (getline(inFile, oneline)){
      outcomeFields.clear();
      activeMemberIdsString.clear();
      activeMemberIds.clear();
      state.clear();
      stateString.clear();

      split(oneline,delim,outcomeFields);

      id = atoi(outcomeFields[0].c_str());

      split(outcomeFields[1],' ',activeMemberIdsString);
      for (int i = 0; i < activeMemberIdsString.size(); i++)
	 activeMemberIds.push_back(atoi(activeMemberIdsString[i].c_str()));

      somedouble_1 = atof(outcomeFields[2].c_str());
      somedouble_2 = atof(outcomeFields[3].c_str());
      somedouble_3 = atof(outcomeFields[4].c_str());

      if (outcomeFields.size() > 5){
	 split(outcomeFields[5],' ',stateString);
	 for (int i = 0; i < stateString.size(); i++)
	    state.push_back(atof(stateString[i].c_str()));
      }

      if (phase == TRAIN_PHASE)//this only makes sense for keepaway
	 _simTime += somedouble_1;

      if (phase != TEST_PHASE){
	 for(teiter = _M.begin(); teiter != _M.end(); teiter++){
	    if ((*teiter)->id() == id){
	       (*teiter)->updateActiveMembersFromIds(activeMemberIds);
	       (*teiter)->setOutcome(new point(point_count, state.size(), t, &state[0], point_count++, somedouble_1,somedouble_2,somedouble_3,phase),somedouble_1,_numStoredOutcomesPerHost[phase]);
	    }
	 }
      }
      else{
	 if (currentChampion->id() != id){
	    oss.str("");
	    oss << "test error: currentChampion->id() != id: " << currentChampion->id() << " != " << id;
	    die(__FILE__, __FUNCTION__, __LINE__, oss.str().c_str());

	 }
	 currentChampion->setOutcome(new point(point_count, state.size(), t, &state[0], point_count++, somedouble_1,somedouble_2,somedouble_3,phase),somedouble_1,_numStoredOutcomesPerHost[phase]);
      }
   }
   inFile.close();

   if (phase == TEST_PHASE){
      map < point *, double > :: iterator myoiter;
      map < point *, double, pointLexicalLessThan > myOutcomes;
      currentChampion->outcomes(myOutcomes,phase);
      oss.str("");
      oss << "soc::test t " << t << " l " << currentChampion->level();
      oss << " simTime " << _simTime;
      oss << " meanOutcomeTest " << currentChampion->getMeanOutcome(TEST_REWARD,TEST_PHASE,1.0);
      oss << " numOutcomesTest " << currentChampion->numOutcomes(TEST_PHASE);
      oss << " allOutcomesTest";
      for(myoiter = myOutcomes.begin(); myoiter != myOutcomes.end(); myoiter++)
	 if(myoiter->first->phase() == TEST_PHASE)
	    oss << " " << myoiter->first->somedouble(TEST_REWARD);
      cout << oss.str() << endl;
      oss.str("");
   }
}

/********************************************************************************************/
void sbbTD::pruneLearners(bool checkOmega) /* If true, prune only if the team size equals _omega. */
{
   set < team *, teamIdComp > :: iterator teiter;

   set < learner * > members, active;
   set < learner * > setdiff;
   set < learner * > :: iterator leiter;

   /* Remove inactive team members from the teams. */

   for(teiter = _M.begin(); teiter != _M.end(); teiter++)
   {
#ifdef MYDEBUG
      cout << "scm::pruneLearners check " << checkOmega << " size " << (*teiter)->size();
#endif

      if((checkOmega == false) || ((*teiter)->size() == _omega))
      {
	 members.clear();
	 active.clear();
	 setdiff.clear();

	 (*teiter)->members(&members); /* Get all members. */
	 (*teiter)->activeMembers(&active); /* Get active members. */

	 /* Inactive members are members \ active. */

	 set_difference(members.begin(), members.end(),
	       active.begin(), active.end(),
	       insert_iterator < set < learner * > > (setdiff, setdiff.begin()));

#ifdef MYDEBUG
	 cout << " members";
	 for(leiter = members.begin(); leiter != members.end(); leiter++)
	    cout << " " << (*leiter)->id();

	 cout << " active";
	 for(leiter = active.begin(); leiter != active.end(); leiter++)
	    cout << " " << (*leiter)->id();

	 cout << " inactive";
	 for(leiter = setdiff.begin(); leiter != setdiff.end(); leiter++)
	    cout << " " << (*leiter)->id();
#endif

	 for(leiter = setdiff.begin(); leiter != setdiff.end() && (*teiter)->size() > 2; leiter++)
	 {
	    (*leiter)->refDec();
	    (*teiter)->removeLearner(*leiter);
	 }
      }

#ifdef MYDEBUG
      cout << " finalsize " << (*teiter)->size() << endl;
#endif
   }
}

/***********************************************************************************************************
 * Read in populations from a checkpoint file. (This whole process needs a rewrite sometime.)
 **********************************************************************************************************/
//long sbbTD::readCheckpoint(int t, int level,int id,int phase,bool file, istream& is){
long sbbTD::readCheckpoint(int phase, istream& is){
   std::pair<std::set<team *, teamIdComp>::iterator,bool> ret;

   long startLevel = 0; //what level we start at after reading checkpoint

   vector < vector < team * > * > mLevelTmp;
   vector < vector < learner * > * > lLevelTmp;

   set < team * , teamIdComp> _MTmp; /* Teams */
   set < learner * > _LTmp; /* Learners. */
   set < learner * > testTeamMembers;

   set < team *, teamIdComp > :: iterator teiter, teiterEnd;
   set < learner * > :: iterator leiter, leiterEnd;

   bool currentPop = false;

   mLevelTmp.push_back(0);
   lLevelTmp.push_back(0);

   vector < learner * > learners;
   vector < team * > teams;

   string oneline;
   char delim=':';
   char *token;
   int i;
   long seed = 0;
   long learner_level = 0;
   long team_level = 0;
   long memberId = 0;
   long max_team_count = -1;
   long max_learner_count = -1;

   vector < string > outcomeFields;

   while (getline(is, oneline)){
      outcomeFields.clear();

      split(oneline,delim,outcomeFields);

      if (outcomeFields[0].compare("seed") == 0){
	 _seed = atol(outcomeFields[1].c_str());
      }
      else if (outcomeFields[0].compare("learnerPop") == 0 || outcomeFields[0].compare("teamPop") == 0){
	 currentPop = true; 
      }
      else if (outcomeFields[0].compare("endLearnerPop") == 0){
	 _L = _LTmp;
	 currentPop = false;
      }
      else if(outcomeFields[0].compare("endTeamPop") == 0){
	 /* In TEST_PHASE all learner nrefs will be set to zero, so we recalculate them here based on
	  * the single test team.
	  */
	 if (phase == TEST_PHASE || phase == PLAY_PHASE){
	    teiter = _MTmp.begin();
	    (*teiter)->members(&testTeamMembers);
	    for(leiter = testTeamMembers.begin(); leiter != testTeamMembers.end(); leiter++)
	       (*leiter)->refInc();
	 }
	 _M = _MTmp;
	 currentPop = false;
      }
      else if (outcomeFields[0].compare("learnerLevel") == 0){
	 learner_level = atol(outcomeFields[1].c_str());
	 if (learner_level > 1){
	    lLevelTmp.push_back(new vector < learner * >);
	    (lLevelTmp.back())->insert((lLevelTmp.back())->begin(), learners.begin(), learners.end());
	    learners.clear();
	 }
      }
      else if (outcomeFields[0].compare("learnerEnd") == 0){
	 lLevelTmp.push_back(new vector < learner * >);
	 (lLevelTmp.back())->insert((lLevelTmp.back())->begin(), learners.begin(), learners.end());
	 learners.clear();
	 startLevel = learner_level;
      }
      else if (outcomeFields[0].compare("learner") == 0){
	 long id = 0;
	 long gtime = 0;
	 long ancestral_gtime;
	 long action = 0;;
	 long dim = 0;
	 int nrefs = 0;
	 int atomic = 0;
	 vector < instruction * > bid;
	 instruction *in;
	 id = atol(outcomeFields[1].c_str());
	 if (id > max_learner_count) max_learner_count = id;
	 gtime = atol(outcomeFields[2].c_str());
	 action = atol(outcomeFields[3].c_str());
	 dim = atol(outcomeFields[4].c_str()); 
	 if (phase < TEST_PHASE) nrefs = atol(outcomeFields[5].c_str()); 
	 else nrefs = 0;
	 atomic = atol(outcomeFields[6].c_str());    

	 for (int ii = 7; ii < outcomeFields.size(); ii++){
	    token = (char*)outcomeFields[ii].c_str();
	    in = new instruction();
	    in->reset();
	    for (int j = 22, k = 0; j >= 0; j--, k++){ //there are 23 bits in each instruction
	       if (token[j] == '1'){
		  in->flip(k);
	       }
	    }
	    bid.push_back(in);
	 }
	 learner *l;
	 l = new learner(gtime, gtime, action, dim, id, nrefs, bid);
	 l->atomic(atomic);
	 if (currentPop == true)
	    _LTmp.insert(l);
	 else
	    learners.push_back(l);
      }

      else if (outcomeFields[0].compare("teamLevel") == 0){
	 team_level = atol(outcomeFields[1].c_str());
	 if (team_level > 1){
	    mLevelTmp.push_back(new vector < team * >);
	    (mLevelTmp.back())->insert((mLevelTmp.back())->begin(), teams.begin(), teams.end());
	    teams.clear();
	 }
      }

      else if (outcomeFields[0].compare("teamEnd") == 0){
	 mLevelTmp.push_back(new vector < team * >);
	 (mLevelTmp.back())->insert((mLevelTmp.back())->begin(), teams.begin(), teams.end());
	 teams.clear();
      }

      else if (outcomeFields[0].compare("team") == 0){
	 long id = 0;
	 long gtime = 0;
	 long level = 0;
	 long numOutcomes = 0;
	 set < learner * > members;
	 int i = 0;
	 team *m;
	 id = atol(outcomeFields[1].c_str());
	 if (id > max_team_count) max_team_count = id;
	 gtime = atol(outcomeFields[2].c_str());
	 level = atol(outcomeFields[3].c_str());
	 numOutcomes = atol(outcomeFields[4].c_str());

	 m = new team(level,mLevelTmp[level],gtime, id);
	 m->tmpNumOutcomes(numOutcomes);
	 for (int ii = 5; ii < outcomeFields.size(); ii++){
	    memberId = atol(outcomeFields[ii].c_str());
	    if (currentPop == true){
	       for(leiter = _L.begin(), leiterEnd = _L.end(); leiter != leiterEnd; leiter++){
		  if ((*leiter)->id() == memberId){
		     m->addLearner(*leiter);
		  }
	       }
	    }
	    else{
	       for(int j = 0; j < lLevelTmp[team_level]->size(); j++){
		  if ((*lLevelTmp[team_level])[j]->id() == memberId){
		     m->addLearner((*lLevelTmp[team_level])[j]);
		  }
	       }
	    }
	 }
	 if (currentPop == true){
	    ret = _MTmp.insert(m);
	    //if we get a duplicate team id, scale by 100 until unique
	    while (ret.second==false){
	       m->id(m->id()*100);
	       ret = _MTmp.insert(m);
	    }
	 }
	 else
	    teams.push_back(m);
      }
   }
   //if ( file)
   //  ss.close();
   _lLevel = lLevelTmp;
   _mLevel = mLevelTmp;
   learner_count = max_learner_count+1;
   team_count = max_team_count+1;
   return startLevel;
}

/*********************************************************************************************************/
void sbbTD::recalculateLearnerRefs(){

   set < learner * > :: iterator leiter, leiterEnd;
   for(leiter = _L.begin(); leiter != _L.end(); leiter++)
      (*leiter)->setNrefs(0);

   set < team * > :: iterator teiter, teiterEnd;
   set < learner * > mem;
   for(teiter = _M.begin(); teiter != _M.end(); teiter++){
      (*teiter)->members(&mem);
      set < learner * > :: iterator leiter;
      for(leiter = mem.begin(); leiter != mem.end(); leiter++)
	 (*leiter)->refInc(); 
      mem.clear();
   }
}

/*********************************************************************************************************/
void sbbTD::scaleTeamAndLearnerIds(long i){

   set < learner * > :: iterator leiter, leiterEnd;
   for(leiter = _L.begin(); leiter != _L.end(); leiter++)
      (*leiter)->id((*leiter)->id()+i);
   set < team * > :: iterator teiter, teiterEnd;
   for(teiter = _M.begin(); teiter != _M.end(); teiter++)
      (*teiter)->id((*teiter)->id()+i);
}

/********************************************************************************************/
void sbbTD::selTeams(long t, long level)
{
   vector < team * > teams;
   set < team *, teamIdComp > :: iterator teiter, teiterend;

   if(_diversityMode == 0){
      //this diversityMode runs in here for reporting
      //paretoRanking(t, level);
      //reset scores to fit only
      diversityMode0(t, level);
   }
   else if (_diversityMode == 7)
      paretoRanking(t, level);
   else if (_diversityMode == 8)
      paretoRankingR1R2(t, level);

   ostringstream tmposs;
   map < point *, double, pointLexicalLessThan > allOutcomes;
   map < point *, double > :: iterator myoiter;

   for(teiter = _M.begin(); teiter != _M.end(); teiter++)
      teams.push_back(*teiter);
   // Select the top teams.
   for(int i = 0; i < teams.size(); i++)
      teams[i]->key(teams[i]->score());
   // Sort the teams by their keys.
   partial_sort(teams.begin(), teams.begin() + _Mgap, teams.end(), lessThan < team > ());
#ifdef MYDEBUG
   for(int i = 0; i < teams.size(); i++)
      cout << "_id " << _id << "selTeams " << t << " sorted " << teams[i]->id() << " key " << teams[i]->key() << endl;
#endif
   // At this point, the order of teams no longer matches the order of the other vectors.
   bool staticGap = true;
   int numOldDeleted = 0;
   int numDeleted = 0;
   if (staticGap == true){
      oss << "sbb::selTeams deletedAge";
      for(int i = 0; i < _Mgap; i++){
	 oss << " " << t - teams[i]->gtime();
	 if(teams[i]->gtime() != t){
	    numOldDeleted++;
	    _mdom++; // The team is old.
	 }
#ifdef MYDEBUG
	 cout << "sbb::selTeams deleting " << teams[i]->id() << endl;
#endif
	 _M.erase(teams[i]);
	 delete teams[i]; // Learner refs deleted automatically.
	 numDeleted++;
      }
   }
   else{
      oss << "sbb::selTeams deletedAge";
      for(int i = 0; i < teams.size(); i++){
	 if (teams[i]->score() == 0){
	    oss << " " << t - teams[i]->gtime();
	    if(teams[i]->gtime() != t){
	       numOldDeleted++;
	       _mdom++; // The team is old.
	    }
#ifdef MYDEBUG
	    cout << "sbb::selTeams deleting " << teams[i]->id() << endl;
#endif
	    _M.erase(teams[i]);
	    delete teams[i]; // Learner refs deleted automatically.
	    numDeleted++;
	 }
      }
   }

   oss << " numDeleted " << numDeleted << " numOldDeleted " << numOldDeleted << " keptAge";
   for(teiter = _M.begin(); teiter != _M.end(); teiter++)
      oss << " " << t - (*teiter)->gtime();
   oss << " keptIds";
   for(teiter = _M.begin(); teiter != _M.end(); teiter++)
      oss << " " << (*teiter)->id();
   oss << endl;
   cout << oss.str();
   oss.str("");
}

/********************************************************************************************/
void sbbTD::setParams()
{
   oss.str("");
   oss << "sbb." << _seed << ".arg";

   map < string, string > args;

   readMap(oss.str(), args);

   oss.str("");
   oss << "sbbTD parameters:" << endl;

   map < string, string > :: iterator maiter;

   /* Get arguments. */

   if((maiter = args.find("paretoEpsilonTeam")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg paretoEpsilonTeam");

   _paretoEpsilonTeam = stringTodouble(maiter->second);

   if((maiter = args.find("teamPow")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg teamPow");

   _teamPow = stringToLong(maiter->second);

   if((maiter = args.find("monolithic")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg monolithic");

   int mono = stringToInt(maiter->second);
   if (mono > 0)
      _monolithic = true;
   else
      _monolithic = false;

   if((maiter = args.find("splitLevel")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg splitLevel");

   int sl = stringToInt(maiter->second);
   if (sl > 0)
      _splitLevel = true;
   else
      _splitLevel = false;

   if((maiter = args.find("Msize")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg Msize");

   _Msize = stringToInt(maiter->second);

   if((maiter = args.find("pmd")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pmd");

   _pmd = stringTodouble(maiter->second);

   if((maiter = args.find("pma")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pma");

   _pma = stringTodouble(maiter->second);

   if((maiter = args.find("pmm")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pmm");

   _pmm = stringTodouble(maiter->second);

   if((maiter = args.find("pmn")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pmn");

   _pmn = stringTodouble(maiter->second);

   if((maiter = args.find("omega")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg omega");

   _omega = stringToInt(maiter->second);

   if((maiter = args.find("Mgap")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg Mgap");

   _Mgap = stringToInt(maiter->second);

   if((maiter = args.find("maxProgSize")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg maxProgSize");

   _maxProgSize = stringToInt(maiter->second);

   if((maiter = args.find("pBidMutate")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pBidMutate");

   _pBidMutate = stringTodouble(maiter->second);

   if((maiter = args.find("pBidSwap")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pBidSwap");

   _pBidSwap = stringTodouble(maiter->second);

   if((maiter = args.find("pBidDelete")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pBidDelete");

   _pBidDelete = stringTodouble(maiter->second);

   if((maiter = args.find("pBidAdd")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pBidAdd");

   _pBidAdd = stringTodouble(maiter->second);

   if((maiter = args.find("diversityMode")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg diversityMode");

   _diversityMode = stringToInt(maiter->second);

   if((maiter = args.find("stateDiscretizationSteps")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg stateDiscretizationSteps");

   _stateDiscretizationSteps = stringToInt(maiter->second);

   if((maiter = args.find("sigmaShare")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg sigmaShare");

   _sigmaShare = stringTodouble(maiter->second);

   if((maiter = args.find("pNoveltyGeno")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pNoveltyGeno");

   _pNoveltyGeno = stringTodouble(maiter->second);

   if((maiter = args.find("pNoveltyPheno")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg pNoveltyPheno");

   _pNoveltyPheno = stringTodouble(maiter->second);

   if((maiter = args.find("knnNovelty")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg knnNovelty");

   _knnNov = stringToInt(maiter->second);

   if((maiter = args.find("validPhaseEpochs")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg validPhaseEpochs");

   _validPhaseEpochs = stringToLong(maiter->second);

   if((maiter = args.find("testPhaseEpochs")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg testPhaseEpochs");

   _testPhaseEpochs = stringToLong(maiter->second);

   if((maiter = args.find("episodesPerGeneration")) == args.end())
      die(__FILE__, __FUNCTION__, __LINE__, "cannot find arg episodesPerGeneration");

   _episodesPerGeneration = stringToLong(maiter->second);

   oss << "seed " << _seed << endl;

   oss << "paretoEpsilonTeam " << _paretoEpsilonTeam << endl;

   oss << "teamPow " << _teamPow << endl;

   oss << "monolithic " << _monolithic << endl;

   oss << "Msize " << _Msize << endl;
   oss << "pmd " << _pmd << endl;
   oss << "pma " << _pma << endl;
   oss << "pmm " << _pmm << endl;
   oss << "pmn " << _pmn << endl;
   oss << "omega " << _omega << endl;
   oss << "numLevels " << _numLevels << endl;
   oss << "Mgap " << _Mgap << endl;

   oss << "maxProgSize " << _maxProgSize << endl;
   oss << "pBidMutate " << _pBidMutate << endl;
   oss << "pBidSwap " << _pBidSwap << endl;
   oss << "pBidDelete " << _pBidDelete << endl;
   oss << "pBidAdd " << _pBidAdd << endl;

   oss << "diversityMode " << _diversityMode << endl;
   oss << "stateDiscretizationSteps " << _stateDiscretizationSteps << endl;
   oss << "pNoveltyGeno " << _pNoveltyGeno << endl;
   oss << "pNoveltyPheno " << _pNoveltyPheno << endl;

   oss << "validPhaseEpochs " << _validPhaseEpochs << endl;
   oss << "testPhaseEpochs " << _testPhaseEpochs << endl;
   oss << "episodesPerGeneration " << _episodesPerGeneration << endl;

   cout << oss.str() << endl;
   oss.str("");

   if(_numLevels < 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg numLevels < 1");

   if(_Msize < 2)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg Msize < 2");

   if(_pmd < 0 || _pmd > 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg pmd < 0 || pmd > 1");

   if(_pma < 0 || _pma > 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg pma < 0 || pma > 1");

   if(_pmm < 0 || _pmm > 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg pmm < 0 || pmm > 1");

   if(_pmn < 0 || _pmn > 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg pmn < 0 || pmn > 1");

   if(_omega < 2)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg omega < 2");

   if(_Mgap < 1 || _Mgap >= _Msize)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg Mgap < 1 || Mgap >= Msize");

   if(_maxProgSize < 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg _maxProgSize < 1");

   if(_pBidDelete < 0 || _pBidDelete > 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg pBidDelete < 0 || pBidDelete > 1");

   if(_pBidAdd < 0 || _pBidAdd > 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg pBidAdd < 0 || pBidAdd > 1");

   if(_pBidSwap < 0 || _pBidSwap > 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg pBidSwap < 0 || pBidSwap > 1");

   if(_pBidMutate < 0 || _pBidMutate > 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg pBidMutate < 0 || pBidMutate > 1");

   if(_paretoEpsilonTeam < 0)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg _paretoEpsilonTeam < 0");

   if(_teamPow < 1)
      die(__FILE__, __FUNCTION__, __LINE__, "bad arg teamPow < 1");
}

/*********************************************************************************************************/
void sbbTD::splitLevelPrep(){
   set < learner * > :: iterator leiter, leiterEnd;
   for(leiter = _L.begin(); leiter != _L.end(); leiter++)
      (*leiter)->action((*leiter)->action()+1);
}

/********************************************************************************************/
void sbbTD::stats(long t,
      long level)
{
   oss.str("");
   int sumTeamSizes = 0;
   int nrefs = 0;
   int numOutcomes = 0;
   int sumNumOutcomes = 0;
   vector <int> allNumOutcomes;

   /* Quartiles for team sizes. */
   vector < int > msize;
   int mq1 = (int) (_M.size() * 0.25);
   int mq2 = (int) (_M.size() * 0.5);
   int mq3 = (int) (_M.size() * 0.75);

   /* Team node counts. */
   vector < int > mnode;

   /* Quartiles for team ref counts, counting references to each learner. */
   vector < int > lrefs;
   vector < int > lsize;
   vector < int > lesize;
   int lrq1 = (int) (_L.size() * 0.25);
   int lrq2 = (int) (_L.size() * 0.5);
   int lrq3 = (int) (_L.size() * 0.75);

   //set < point * > :: iterator poiter;
   set < learner * > :: iterator leiter;
   set < team *, teamIdComp > :: iterator teiter;

   double outcome = 0;

   /* Mean outcome for a team. */
   double meanOutcome[3]={0,0,0};

   double meanChildUp = 0;

   double maxMeanOutcome[3]={0,0,0};
   double meanMeanOutcome[3]={0,0,0};
   double minMeanOutcome[3]={numeric_limits<double>::max(),numeric_limits<double>::max(),numeric_limits<double>::max()};
   int numOutcomesForMaxMean[3]={0,0,0};
   int numOutcomesForMinMean[3]={0,0,0};
   int maxMeanSize[3]={0,0,0};
   int minMeanSize[3]={0,0,0};
   long maxMeanGtime[3]={0,0,0};
   long minMeanGtime[3]={0,0,0};

   long gtime = 0;

   long minGtime = numeric_limits<double>::max();
   double meanAge = 0;

   vector < double > state;

   vector < double > outcomes;
   map < point *, vector < short > * > dist;
   map < point *, vector < short > * > :: iterator diiter;
   vector < short > *dvec;
   long ndist;

   set < team *, teamIdComp > :: iterator teiter1, teiter2;
   set < learner * > lset1, lset2, linter;
   int i1, i2;
   map < int, int > olaphist; /* Overlap histogram. */

   oss << "scm::symStat lrefs";
   for(leiter = _L.begin(); leiter != _L.end(); leiter++)
   {
      nrefs += (*leiter)->refs();
      lrefs.push_back((*leiter)->refs());
      lsize.push_back((*leiter)->size());
      lesize.push_back((*leiter)->esize());

      oss << " " << (*leiter)->id() << ":" << (*leiter)->refs();
   }
   oss << endl;

   for(teiter = _M.begin(); teiter != _M.end(); teiter++)
   {
      msize.push_back((*teiter)->size());
      mnode.push_back((*teiter)->nodes());
      sumTeamSizes += (*teiter)->size();
      gtime = (*teiter)->gtime();
      allNumOutcomes.push_back((*teiter)->numOutcomes(TRAIN_PHASE));

      for (int i = 0; i < 3; i++){
	 meanOutcome[i] = (*teiter)->getMeanOutcome(i+1,TRAIN_PHASE,MEAN_OUT_PROP);
	 meanMeanOutcome[i] += meanOutcome[i];

	 if(meanOutcome[i] > maxMeanOutcome[i]){
	    maxMeanOutcome[i] = meanOutcome[i];
	    maxMeanGtime[i] = (*teiter)->gtime();
	    numOutcomesForMaxMean[i] = (*teiter)->numOutcomes(TRAIN_PHASE);
	    maxMeanSize[i] = (*teiter)->size();
	 }
	 if(meanOutcome[i] < minMeanOutcome[i]){
	    minMeanOutcome[i] = meanOutcome[i];
	    minMeanGtime[i] = (*teiter)->gtime();
	    numOutcomesForMinMean[i] = (*teiter)->numOutcomes(TRAIN_PHASE);
	    minMeanSize[i] = (*teiter)->size();
	 }
      }
      if (gtime < minGtime)
	 minGtime = gtime;
      meanAge += t-gtime;

      if ((*teiter)->score() > (*teiter)->parentScore())
	 meanChildUp++;
   }

   sort(msize.begin(), msize.end());
   sort(mnode.begin(), mnode.end());
   sort(lrefs.begin(), lrefs.end());
   sort(lsize.begin(), lsize.end());
   sort(lesize.begin(), lesize.end());

   oss << "scm::stats seed " << _seed << " t " << t << " lev " << level;
   //oss << " mdom " << _mdom;
   //oss << " pdom " << _pdom;
   oss << " Lsize " << _L.size();
   oss << " msize " << msize[0] << " " << msize[mq1] << " " << msize[mq2] << " " << msize[mq3] << " " << msize.back();
   oss << " mnode " << mnode[0] << " " << mnode[mq1] << " " << mnode[mq2] << " " << mnode[mq3] << " " << mnode.back();
   oss << " lrefs " << lrefs[0] << " " << lrefs[lrq1] << " " << lrefs[lrq2] << " " << lrefs[lrq3] << " " << lrefs.back();
   oss << " lsize " << lsize[0] << " " << lsize[lrq1] << " " << lsize[lrq2] << " " << lsize[lrq3] << " " << lsize.back();
   oss << " lesize " << lesize[0] << " " << lesize[lrq1] << " " << lesize[lrq2] << " " << lesize[lrq3] << " " << lesize.back();

   oss << " meanMeanOut"; for (int i=0; i<3; i++) oss << " " << meanMeanOutcome[i]/_M.size();

   oss << " maxMeanOut"; for (int i=0; i<3; i++) oss << " " << maxMeanOutcome[i];
   oss << " maxMeanAge"; for (int i=0; i<3; i++) oss << " " << t - maxMeanGtime[i];
   oss << " numOutcomesForMaxMean"; for (int i=0; i<3; i++) oss << " " << numOutcomesForMaxMean[i];
   oss << " maxMeanSize"; for (int i=0; i<3; i++) oss << " " << maxMeanSize[i];

   oss << " minMeanOut"; for (int i=0; i<3; i++) oss << " " << minMeanOutcome[i];
   oss << " minMeanAge"; for (int i=0; i<3; i++) oss << " " << t - minMeanGtime[i];
   oss << " numOutcomesForMinMean"; for (int i=0; i<3; i++) oss << " " << numOutcomesForMinMean[i];
   oss << " minMeanSize"; for (int i=0; i<3; i++) oss << " " << minMeanSize[i];

   oss << " maxAge " << t - minGtime;
   oss << " simTime " << _simTime/_numActions; //_simTime / numder of players (number of actions)
   oss << " nrefs " << nrefs;
   oss << " meanAge " << meanAge/_M.size();
   oss << " meanChildUp " << meanChildUp/_M.size();
   oss << " allNumOutcomes " << vecToStr(allNumOutcomes);
   oss << endl;

   /* Perform the following check to make sure that the number of
      references recorded in the learners is actually equal to the sum
      of the team sizes and that the outcome counts equal the number of
      points multiplied by the number of teams. */

   //if(sumTeamSizes != nrefs || sumNumOutcomes != (_Msize - _Mgap) * _episodesPerGeneration) //(_Psize - _Pgap))
   if(sumTeamSizes != nrefs)
   {
      //cout <<"sumNumOutcomes " << sumNumOutcomes << " though it'd be " << (_Msize - _Mgap) * _episodesPerGeneration << endl;
      cout << "sumteamsizes " << sumTeamSizes << " nrefs " << nrefs << endl;
      die(__FILE__, __FUNCTION__, __LINE__, "something does not add up"); 
   }


   /* Team overlap. */

   /* Maps shared learner count to how many teams share that many learners. */

   for(int i = 0; i <= _omega; i++)
      olaphist.insert(map < int, int > :: value_type(i, 0));

   oss << "scmImplicit::stats seed " << _seed << " t " << t << " lev " << level;
   oss << " sharedlearners " << _M.size() << " ids";
   for(teiter1 = _M.begin(); teiter1 != _M.end(); teiter1++)
      oss << " " << (*teiter1)->id();
   oss << " matrix";

   for(i1 = 0, teiter1 = _M.begin(); teiter1 != _M.end(); i1++, teiter1++)
   {
      for(i2 = 0, teiter2 = _M.begin(); teiter2 != _M.end(); i2++, teiter2++)
      {
	 if(i2 < i1) /* Already considered this pair. */
	 {
	    oss << " 0";
	 }
	 else if(i2 > i1) /* Haven't considered this pair yet. */
	 {
	    lset1.clear();
	    lset2.clear();
	    linter.clear();

	    (*teiter1)->members(&lset1);
	    (*teiter2)->members(&lset2);

	    set_intersection(lset1.begin(), lset1.end(),
		  lset2.begin(), lset2.end(),
		  insert_iterator < set < learner * > > (linter, linter.begin()));

	    oss << " " << linter.size();

	    olaphist[linter.size()]++;
	 }
	 else /* Same team, i1 == i2. */
	 {
	    oss << " " << (*teiter1)->size();
	 }
      }
   }
   oss << endl;

   oss << "scmImplicit::stats id " << _id << " seed " << _seed << " t " << t << " lev " << level << " olaphist mat";
   for(int i = 0; i <= _omega; i++)
      oss << " " << olaphist[i];
   oss << endl;
   cout << oss.str();
   oss.str("");
}

/**********************************************************************************************************/
void sbbTD::testSymbiontUtilityDistance(){
   ostringstream o;
   vector< team* > teams;
   set < team * , teamIdComp > :: iterator teiter;
   for(teiter = _M.begin(); teiter != _M.end(); teiter++)
      teams.push_back(*teiter);
   for (int i = 0; i < teams.size(); i++)
      for (int j = 0; j < teams.size(); j++){
	 double dist = teams[i]->symbiontUtilityDistance(teams[j],_omega);
      }
}

/***********************************************************************************************************/
void sbbTD::writeCheckpoint(int phase,ostream& os) const {
   set < team *, teamIdComp > :: iterator teiter, itBegin, teiterEnd;
   set < learner * > :: iterator leiter, leiterEnd;

   os << "seed:" << _seed << endl;

   int i,j;
   //stored populations from previous levels
   if (_lLevel.size() > 1 && _mLevel.size() > 1){
      for(i = 1; i < _lLevel.size(); i++)
      {
	 os << "learnerLevel:" << i << endl;
	 for(j = 0; j < _lLevel[i]->size(); j++)
	    os <<  (*_lLevel[i])[j]->checkpoint();
      }
      os << "learnerEnd" << endl;
      for(i = 1; i < _mLevel.size(); i++)
      {
	 os << "teamLevel:" << i << endl;
	 for(j = 0; j < _mLevel[i]->size(); j++)
	    os <<  (*_mLevel[i])[j]->checkpoint(phase);
      }
      os << "teamEnd" << endl;
   }
   //current populations
   os << "learnerPop:" << endl;
   for(leiter = _L.begin(), leiterEnd = _L.end(); leiter != leiterEnd; leiter++)
      os << (*leiter)->checkpoint();
   os << "endLearnerPop" << endl;
   os << "teamPop:" << endl;
   for(teiter = _M.begin(), teiterEnd = _M.end(); teiter != teiterEnd; teiter++){
      if (phase == TEST_PHASE){
	 if ((*teiter)->id() == currentChampion->id())
	    os << (*teiter)->checkpoint(phase);
      }
      else if (phase == SINGLE_EVAL_PHASE){
	 if ((*teiter)->id() == (*teiterEval)->id())
	    os << (*teiter)->checkpoint(phase);
      }
      else
	 os << (*teiter)->checkpoint(phase);
   }
   os << "endTeamPop" << endl;
}

/***********************************************************************************************************/
void sbbTD::writeEval(int t, int level, int phase){
   ofstream outFile;
   char outputFilenameTmp[80];
   char outputFilename[80];
   //mkdir("evals", S_IRWXU|S_IRGRP|S_IXGRP);
   sprintf(outputFilenameTmp, "evals/tmp.eval.%d.%d.%d.rslt",phase,_id,_seed );
   sprintf(outputFilename, "evals/eval.%d.%d.%d.rslt",phase,_id,_seed );

   if (fileExists(outputFilename))
      rename(outputFilename,outputFilenameTmp);

   outFile.open(outputFilenameTmp, ios::out);

   if (!outFile) {
      cerr << "Can't open eval file write: " << outputFilenameTmp << endl;
      exit(1);
   }
   map < point *, double, pointLexicalLessThan > allOutcomes;
   map < point *, double > :: iterator myoiter;
   vector <double> episodeProfile;
   set < team * > :: iterator teiter, teiterEnd;
   set < learner * > activeMembers;
   set < learner * > :: iterator leiter;

   for(teiter = _M.begin(), teiterEnd = _M.end(); teiter != teiterEnd; teiter++){
      (*teiter)->outcomes(allOutcomes,phase);
      for(myoiter = allOutcomes.begin(); myoiter != allOutcomes.end(); myoiter++){
	 (myoiter->first)->state(episodeProfile);
	 outFile << (*teiter)->id() << ":";
	 activeMembers.clear();
	 (*teiter)->activeMembers(&activeMembers); /* Get active members. */
	 for(leiter = activeMembers.begin(); leiter != activeMembers.end(); leiter++){
	    if (leiter != activeMembers.begin())
	       outFile << " ";
	    outFile << (*leiter)->id();
	 }
	 outFile << ":" << (myoiter->first)->somedouble(1) << ":" << (myoiter->first)->somedouble(2) << ":" << (myoiter->first)->somedouble(3) << ":" << vecToStr(episodeProfile) << endl;
      }
   }
   outFile.close();
   rename(outputFilenameTmp,outputFilename);
}

/***********************************************************************************************************/
bool sbbTD::writeRunShare(double f){
   ofstream outFile;
   outFile.precision(numeric_limits< double >::digits10+1);
   char outputFilename[80];
   char fileToDelete[80];
   sprintf( outputFilename, "run_share/medianOutcome.%d.rslt",_seed);
   if (fileExists(outputFilename) == false){
      outFile.open(outputFilename, ios::out);
      outFile << f << endl;
      outFile.close();
      return true;
   }
   return false;
}
