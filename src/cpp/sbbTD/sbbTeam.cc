#include <algorithm>
#include <limits>
#include "sbbTeam.h"
long team::_count = 0;
/********************************************************************************************/
bool team::addLearner(learner *lr)
{
   bool added;

   added = (_members.insert(lr)).second;

   if(added == true) /* Update node count. */
   {
      if (lr->atomic() || _level == 0){
	 _nodes += 1; /* Leaf. */
      }
      else {/* Added a team. */
	 _nodes += (*_actions)[lr->action()]->nodes(); /* Internal node. */
      }
   }

   return added;
}

/********************************************************************************************/
string team::checkpoint(int phase){
   ostringstream oss;
   set < learner * > :: iterator leiter;

   oss << "team:" << _id << ":" << _gtime << ":" << _level << ":" << numOutcomes(phase);
   for(leiter = _members.begin(); leiter != _members.end(); leiter++)
      oss << ":" << (*leiter)->id();

   oss << endl;

   return oss.str();
}

ostream & operator<<(ostream &os, 
      const team &tm)
{
   set < long > features;
   set < long > :: iterator seiter;
   long sumfeat, sumlsize, sumlesize;

   set < learner * > :: iterator leiter;

   tm.features(features);

   for(sumfeat = 0, sumlsize = 0, sumlesize = 0,
	 leiter = tm._members.begin(); leiter != tm._members.end(); leiter++)
   {
      sumfeat += (*leiter)->numFeatures();
      sumlsize += (*leiter)->size();
      sumlesize += (*leiter)->esize();
   }

   os << "teamout";
   os << " id " << tm.id();
   os << " size " << tm.size();
   os << " active " << tm.asize();
   os << " nodes " << tm.nodes();
   os << " gtime " << tm.gtime();
   os << " feat " << sumfeat << " " << (double) sumfeat / tm.size();
   os << " uniqfeat " << features.size() << " " << (double) features.size() / tm.size();
   os << " lsize " << sumlsize << " " << (double) sumlsize / tm.size();
   os << " lesize " << sumlesize << " " << (double) sumlesize / tm.size();

   os << " id";
   for(leiter = tm._members.begin(); leiter != tm._members.end(); leiter++){
      //uintptr_t ptr_val;// = uintptr_t(*leiter); 
      os << " " << (*leiter)->id();// << "(" << (long long)(*leiter) << ")";
   }


   os << " size";
   for(leiter = tm._members.begin(); leiter != tm._members.end(); leiter++)
      os << " " << (*leiter)->size();

   os << " esize";
   for(leiter = tm._members.begin(); leiter != tm._members.end(); leiter++)
      os << " " << (*leiter)->esize();

   os << " gtime";
   for(leiter = tm._members.begin(); leiter != tm._members.end(); leiter++)
      os << " " << (*leiter)->gtime();

   os << " refs";
   for(leiter = tm._members.begin(); leiter != tm._members.end(); leiter++)
      os << " " << (*leiter)->refs();

   os << " action";
   for(leiter = tm._members.begin(); leiter != tm._members.end(); leiter++)
      os << " " << (*leiter)->action();

   os << " numfeat";
   for(leiter = tm._members.begin(); leiter != tm._members.end(); leiter++)
      os << " " << (*leiter)->numFeatures();

   os << " lfeat";
   for(leiter = tm._members.begin(); leiter != tm._members.end(); leiter++)
   {
      os << " lact " << (*leiter)->action() << " lfid";
      features.clear();
      (*leiter)->features(features);
      for(seiter = features.begin(); seiter != features.end(); seiter++)
	 os << " " << *seiter;
   }

   features.clear();
   tm.features(features);

   os << " featid";
   for(seiter = features.begin(); seiter != features.end(); seiter++)
      os << " " << *seiter;

   //output the same info for active members only
   os << " idA";
   for(leiter = tm._active.begin(); leiter != tm._active.end(); leiter++)
      os << " " << (*leiter)->id();

   os << " sizeA";
   for(leiter = tm._active.begin(); leiter != tm._active.end(); leiter++)
      os << " " << (*leiter)->size();

   os << " esizeA";
   for(leiter = tm._active.begin(); leiter != tm._active.end(); leiter++)
      os << " " << (*leiter)->esize();

   os << " gtimeA";
   for(leiter = tm._active.begin(); leiter != tm._active.end(); leiter++)
      os << " " << (*leiter)->gtime();

   os << " refsA";
   for(leiter = tm._active.begin(); leiter != tm._active.end(); leiter++)
      os << " " << (*leiter)->refs();

   os << " actionA";
   for(leiter = tm._active.begin(); leiter != tm._active.end(); leiter++)
      os << " " << (*leiter)->action();

   os << " numfeatA";
   for(leiter = tm._active.begin(); leiter != tm._active.end(); leiter++)
      os << " " << (*leiter)->numFeatures();

   os << " lfeatA";
   for(leiter = tm._active.begin(); leiter != tm._active.end(); leiter++)
   {
      os << " lactA " << (*leiter)->action() << " lfidA";
      features.clear();
      (*leiter)->features(features);
      for(seiter = features.begin(); seiter != features.end(); seiter++)
	 os << " " << *seiter;
   }

   features.clear();
   tm.features(features);

   os << " featidA";
   for(seiter = features.begin(); seiter != features.end(); seiter++)
      os << " " << *seiter;

   return os;
}

/********************************************************************************************/
long team::collectiveAge(long t){
   set < learner * > :: iterator leiter;
   long age = 0;
   for(leiter = _members.begin(); leiter != _members.end(); leiter++)
      age+= t - (*leiter)->gtime();
   return age;
}

/********************************************************************************************/
void team::deleteOutcome(point *pt)
{
   map < point *, double > :: iterator ouiter;

   if((ouiter = _outcomes.find(pt)) == _outcomes.end())
      die(__FILE__, __FUNCTION__, __LINE__, "should not delete outcome that is not set");

   _outcomes.erase(ouiter);
}

/********************************************************************************************/
void team::deleteMargin(point *pt)
{
   map < point *, double > :: iterator maiter;

   if((maiter = _margins.find(pt)) == _margins.end())
      die(__FILE__, __FUNCTION__, __LINE__, "should not delete margin that is not set");

   _margins.erase(maiter);
}

/********************************************************************************************/
void team::features(set < long > &F) const
{
   set < learner * > :: iterator leiter;

   if(F.empty() == false)
      die(__FILE__, __FUNCTION__, __LINE__, "feature set not empty");

   for(leiter = _members.begin(); leiter != _members.end(); leiter++)
      (*leiter)->features(F);
}

/********************************************************************************************/
double team::nov(int type, int kNN){       
   multiset<double>::iterator it;
   double nov = 0;
   int i = 0;
   if (type==0){
      for (it=_distances_0.begin(); it!=_distances_0.end() && i<=kNN; ++it, i++)
	 nov += *it;
      return nov/i;
   }
   else if (type==1){
      for (it=_distances_1.begin(); it!=_distances_1.end() && i<=kNN; ++it, i++)
	 nov += *it;
      return nov/i;
   }
}

/********************************************************************************************/
double team::symbiontUtilityDistance(team * t, long omega){
   vector<int> symbiontIntersection(omega);
   vector<int> symbiontUnion(omega);
   vector<int>::iterator it;
   int symIntersection;
   int symUnion;
   vector < long > team1Ids;
   vector < long > team2Ids;
   set < learner * > activeMembers;
   t->activeMembers(&activeMembers);
   /* if either team has no active members then return 0 */
   if (_active.size() < 1 || activeMembers.size() < 1)
      return 0.0;
   set < learner * > :: iterator leiter;
   for(leiter = _active.begin(); leiter != _active.end(); leiter++)
      team1Ids.push_back((*leiter)->id());
   for(leiter = activeMembers.begin(); leiter != activeMembers.end(); leiter++)
      team2Ids.push_back((*leiter)->id());
   sort(team1Ids.begin(), team1Ids.end());
   sort(team2Ids.begin(), team2Ids.end());
   symbiontIntersection.clear();
   set_intersection (team1Ids.begin(), team1Ids.end(), team2Ids.begin(), team2Ids.end(), back_inserter(symbiontIntersection));
   symIntersection = symbiontIntersection.size();
   symbiontUnion.clear();
   set_union (team1Ids.begin(), team1Ids.end(), team2Ids.begin(), team2Ids.end(), back_inserter(symbiontUnion));
   symUnion = symbiontUnion.size();
#ifdef MYDEBUG
   cout << "genoDiffa t1Size " << _active.size() << " t1Ids " << vecToStr(team1Ids);
   cout << " allMembersSize " << _members.size();
   cout << " t2Size " << t->asize() << " t2Ids ";
   cout << vecToStr(team2Ids) << " symIntersection " << vecToStr(symbiontIntersection) << " symIntersectionSize " << symIntersection;
   cout << " symUnion " << vecToStr(symbiontUnion) << " symUnionSize " << symUnion << " diff " << 1.0 - ((double)symIntersection / (double)symUnion) << endl;
#endif
   return 1.0 - ((double)symIntersection / (double)symUnion);
}
/********************************************************************************************/
//this version compares with a vector of Ids *assumed sorted*
double team::symbiontUtilityDistance(vector < long > & compareWithThese, long omega){
   vector<int> symbiontIntersection(omega);
   vector<int> symbiontUnion(omega);
   vector<int>::iterator it;
   int symIntersection;
   int symUnion;
   vector < long > team1Ids;
   /* if either team has no active members then return 0 */
   if (_active.size() < 1 || compareWithThese.size() < 1)
      return 0.0; 
   set < learner * > :: iterator leiter;
   for(leiter = _active.begin(); leiter != _active.end(); leiter++)
      team1Ids.push_back((*leiter)->id());
   sort(team1Ids.begin(), team1Ids.end());
   symbiontIntersection.clear();
   set_intersection (team1Ids.begin(), team1Ids.end(), compareWithThese.begin(), compareWithThese.end(), back_inserter(symbiontIntersection));
   symIntersection = symbiontIntersection.size();
   symbiontUnion.clear();
   set_union (team1Ids.begin(), team1Ids.end(), compareWithThese.begin(), compareWithThese.end(), back_inserter(symbiontUnion));
   symUnion = symbiontUnion.size();
#ifdef MYDEBUG
   cout << "genoDiffb t1Size " << _active.size() << " t1Ids " << vecToStr(team1Ids);
   cout << " allMembersSize " << _members.size();
   cout << " t2Size " << compareWithThese.size() << " t2Ids ";
   cout << vecToStr(compareWithThese) << " symIntersection " << vecToStr(symbiontIntersection) << " symIntersectionSize " << symIntersection;
   cout << " symUnion " << vecToStr(symbiontUnion) << " symUnionSize " << symUnion << " diff " << 1.0 - ((double)symIntersection / (double)symUnion) << endl;
#endif
   return 1.0 - ((double)symIntersection / (double)symUnion);
}

/********************************************************************************************/
long team::getAction(vector < double > &state,
      vector < learner * > &winner,
      vector < double > &bid1,
      vector < double > &bid2,
      bool updateActive)
{
   //cout.precision(numeric_limits< double >::digits10+1);
   /*
      Returns the index of the action to be taken in the environment, i.e., the
      action of the winning bidder in the level 0 team.

      The vector element winner[i] contains the winning learner at level i so the
      return value should be the same as winner[0]->action() (bid1, bid2, analogous).
    */
   if(_members.size() < 2)
      die(__FILE__, __FUNCTION__, __LINE__, "team is too small");

   set < learner * > :: iterator leiter, wiiter, leiterend;

   double maxBid1; /* Highest bid. */
   double maxBid2 = - HUGE_VAL; /* Second highest bid. */
   double nextBid;

   double *REG = (double *) alloca(REGISTERS * sizeof(double));

   /* Initialize highest bid to that of first learner. */

   wiiter = leiter = _members.begin();
   maxBid1 = (*wiiter)->bid(&state[0], REG);

#ifdef MYDEBUG
   cout << "team::getAction id " << _id << " lev " << _level;
   cout << " winner"; for(int i = 0; i < winner.size(); i++) cout << " " << winner[i]->id();
   cout << " bid1" << vecToStr(bid1) << " bid2" << vecToStr(bid2);
   cout << " bids " << (*wiiter)->id() << "->" << (*wiiter)->action() << "->" << maxBid1;
#endif

   for(leiter++, leiterend = _members.end(); /* Go through the rest of the learners. */
	 leiter != leiterend; leiter++)
   {
      nextBid = (*leiter)->bid(&state[0], REG);

#ifdef MYDEBUG
      cout << " " << (*leiter)->id() << "->" << (*leiter)->action() << "->" << nextBid;
#endif

      if(nextBid > maxBid1) /* Found new highest bidder. */
      {
	 maxBid2 = maxBid1;
	 maxBid1 = nextBid;

	 wiiter = leiter;
      }
      else if(nextBid > maxBid2) /* Found new second highest bidder. */
      {
	 maxBid2 = nextBid;
      }
   }

   bid1.insert(bid1.begin(), maxBid1);
   bid2.insert(bid2.begin(), maxBid2);

   winner.insert(winner.begin(), *wiiter);

   /* Mark this learner as active. */
   if(updateActive == true)
   {
      _active.insert(*wiiter);
   }

#ifdef MYDEBUG
   cout << " winner learnerid " << (*wiiter)->id();
   if(_level != 0)
      cout << " teamid " << (*_actions)[(*wiiter)->action()]->id();
   else
      cout << " teamid -1";
   cout << " bid1 " << maxBid1 << " bid2 " << maxBid2;
   cout << endl;
#endif

   if((*wiiter)->atomic() == true || _level == 0)
      return (*wiiter)->action(); /* Just return the action. */

   /* Repeate at the chosen team in the level below (don't update active learners). */
   return (*_actions)[(*wiiter)->action()]->getAction(state, winner, bid1, bid2, false);
}

/********************************************************************************************/
// This version populates learnersRanked with all learners at each level, sorted by LearnerBidLexicalCompare
long team::getAction(vector < double > &state,
      bool updateActive,
      vector <set <learner *, LearnerBidLexicalCompare> > &learnersRanked, long &decisionInstructions, vector <long> &policyTreeTraceIds)
{
   /*
      Returns the index of the action to be taken in the environment, i.e., the
      action of the winning bidder in the level 0 team.
    */
   if(_members.size() < 2)
      die(__FILE__, __FUNCTION__, __LINE__, "team is too small");

   set < learner * > :: iterator leiter, wiiter, leiterend;

   double *REG = (double *) alloca(REGISTERS * sizeof(double));

   for(leiter = _members.begin(), leiterend = _members.end(); /* Go through all the learners. */
	 leiter != leiterend; leiter++)
   {
      (*leiter)->key((*leiter)->bid(&state[0], REG));
      decisionInstructions += (*leiter)->esize();
   }
   learnersRanked[_level].insert(_members.begin(),_members.end());
   if(updateActive)
   {
      _active.insert(*(learnersRanked[_level].begin()));
   }

   if(_level == 0 || (*(learnersRanked[_level].begin()))->atomic())
      return (*(learnersRanked[_level].begin()))->action();

   /* Repeate at the chosen team in the level below (don't update active learners). */
   //return (*_actions)[(*wiiter)->action()]->getAction(state, false, learnersRanked);
   policyTreeTraceIds.push_back((*_actions)[(*(learnersRanked[_level].begin()))->action()]->id());
   return (*_actions)[(*(learnersRanked[_level].begin()))->action()]->getAction(state, updateActive, learnersRanked, decisionInstructions, policyTreeTraceIds);
}

/********************************************************************************************/
// Fill F with every feature indexed by every learner in this policy (tree).If we ever build 
// massive policy tress, this should be changed to a more efficient traversal. For now just 
// look at every node.
int team::policyFeatures(set < long > &F, bool active)
{
   set < learner * > :: iterator leiter, leiterend;
   set < long > featuresSingle;
   set < long > :: iterator feiter;
   int numLearnersInPolicy = 0;
   
   set < learner * > searchSet;
   if (active)
      searchSet = _active;
   else
      searchSet = _members;

   for(leiter = searchSet.begin(), leiterend = searchSet.end(); leiter != leiterend; leiter++)
   {
      numLearnersInPolicy++;
      featuresSingle.clear();
      (*leiter)->features(featuresSingle);
      F.insert(featuresSingle.begin(),featuresSingle.end());

      //cout << "policyFeatures lev " << _level << " tm " << _id << " numFeatSingle " << featuresSingle.size() << " feat";
      //for(feiter = featuresSingle.begin(); feiter != featuresSingle.end(); feiter++)
      //   cout << " " << *feiter;
      //cout << " numPFeat " << F.size() << " pFeat";
      //for(feiter = F.begin(); feiter != F.end(); feiter++)
      //   cout << " " << *feiter;
      //cout << endl;

      if (_level > 0)
	 numLearnersInPolicy += (*_actions)[(*leiter)->action()]->policyFeatures(F, active); 
   }
   return numLearnersInPolicy;
}

/********************************************************************************************/
// Count all effective instructions in every learner in this policy (tree). Make sure to 
// mark introns first.
int team::policyInstructions(bool effective)
{
   set < learner * > :: iterator leiter, leiterend;
   int numInstructions = 0;

   for(leiter = _members.begin(), leiterend = _members.end(); leiter != leiterend; leiter++)
   {
      if (effective)
	 numInstructions += (*leiter)->esize();
      else
	 numInstructions += (*leiter)->size();

      if (_level > 0)
	 numInstructions += (*_actions)[(*leiter)->action()]->policyInstructions(effective);
   }
   return numInstructions;
}

/********************************************************************************************/
// Collect all learner ids in this policy (tree). active test isn't implemented yet.
int team::policyHosts(vector <long> &ids, int level, bool active)
{
   set < learner * > :: iterator leiter, leiterend;
   int numHosts = 0;
   if (_level == level){
      ids.push_back(_id);
      numHosts++;
   }
   else if (_level > 0){
      if (active){
	 for(leiter = _active.begin(), leiterend = _active.end(); leiter != leiterend; leiter++)
	    numHosts += (*_actions)[(*leiter)->action()]->policyHosts(ids, level, active);
      }
      else{
	 for(leiter = _members.begin(), leiterend = _members.end(); leiter != leiterend; leiter++)
	    numHosts += (*_actions)[(*leiter)->action()]->policyHosts(ids, level, active);
      }
   }  

   return numHosts;
}

/********************************************************************************************/
void team::getBehaviourSequence(vector<int>&s, int phase){
   vector <double> singleEpisodeBehaviour;
   map < point *, double >::iterator it;
   for (it=_outcomes.begin(); it!=_outcomes.end(); it++){
      if ((it->first)->phase() == phase){
	 (it->first)->state(singleEpisodeBehaviour);
	 s.insert(s.end(),singleEpisodeBehaviour.begin(),singleEpisodeBehaviour.end()); //will cast double to int (only discrete values used)
      }
   }
}

/********************************************************************************************/
bool team::getMargin(point *pt,
      double *margin)
{
   map < point *, double > :: iterator maiter;

   if((maiter = _margins.find(pt)) == _margins.end())
      return false;

   *margin = maiter->second;

   return true;
}

/********************************************************************************************/
double team::getMaxOutcome(int i, int phase)
{
   double maxOut = 0;
   map < point *, double > :: iterator ouiter;
   for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++)
      if ((ouiter->first)->phase() == phase && (ouiter->first)->somedouble(i) > maxOut)
	 maxOut = (ouiter->first)->somedouble(i);
   return maxOut;
}

/********************************************************************************************/
/* Get the mean action profile over all stored outcomes.
 * NOTE: Could modify this function to ignore episodes where the keeper didn't touch the ball.*/
void team::getMeanActionProfileAndMeanReward(vector < double > &profile, double *meanReward, int numEpisodeProfileFeatures)
{
   double sumReward;
   vector <double> meanProfile;
   for (int i = 0; i < numEpisodeProfileFeatures; i++){
      meanProfile.push_back(0);
   }
   vector <double> singleState;
   map < point *, double > :: iterator ouiter;
   for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++){
      sumReward += ouiter->second;
      (ouiter->first)->state(singleState);
      for (int i = 0; i < numEpisodeProfileFeatures; i++)
	 meanProfile[i] += singleState[i];
   }
   *meanReward = (double)sumReward/_outcomes.size();
   for (int i = 0; i < numEpisodeProfileFeatures; i++)
      meanProfile[i] = meanProfile[i]/_outcomes.size();
   profile = meanProfile;

}

/********************************************************************************************/
double team::getMinOutcome(int i, int phase)
{
   double minOut = HUGE_VAL;
   map < point *, double > :: iterator ouiter;
   for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++)
      if ((ouiter->first)->phase() == phase && (ouiter->first)->somedouble(i) < minOut)
	 minOut = (ouiter->first)->somedouble(i);
   return minOut;
}

/********************************************************************************************/
double team::getMeanOutcome(int i, int phase, double topPortion)
{
   vector < double > outcomes;
   double sum = 0;
   if (_outcomes.size() == 0)
      return sum;
   map < point *, double > :: iterator ouiter;
   for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++){
      if ((ouiter->first)->phase() == phase)
	 outcomes.push_back((ouiter->first)->somedouble(i));
   }
   sort (outcomes.begin(), outcomes.end(), greater<double>());
   return accumulate(outcomes.begin(),outcomes.begin()+(int)(topPortion*outcomes.size()),0.0)/(int)(topPortion*outcomes.size());
}

/********************************************************************************************/
double team::getMedianOutcome(int i, int phase)
{
   vector <double> outs;
   if (_outcomes.size() == 0)
      return 0;
   map < point *, double > :: iterator ouiter;
   for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++){
      if ((ouiter->first)->phase() == phase)
	 outs.push_back((ouiter->first)->somedouble(i));
   }
   return vecMedian(outs);
}

/********************************************************************************************/
/* Get the ncd and reward for the closest  single-episode behaviourProfile in _outcomes */
void team::getMostSimilarBehaviouralOutcome(vector < double > &compareState, double *out, double *dist)
{
   double minDist = HUGE_VAL;
   double distTmp;
   map < point *, double > :: iterator ouiter;
   vector <double> theirState;
   vector <double> foundState;
   vector <int> compareStateInt;
   vector <int> theirStateInt;
   compareStateInt.insert(compareStateInt.end(),compareState.begin(),compareState.end());
   for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++){
      (ouiter->first)->state(theirState);
      theirStateInt.clear();
      theirStateInt.insert(theirStateInt.end(),theirState.begin(),theirState.end());
      distTmp = normalizedCompressionDistance(compareStateInt, theirStateInt);
      if (distTmp < minDist){
	 (ouiter->first)->state(foundState);
	 *out = ouiter->second;
	 *dist = distTmp;
	 minDist = distTmp;
      }
   }
}

/********************************************************************************************/
bool team::getOutcome(point *pt,
      double *out)
{
   map < point *, double > :: iterator ouiter;

   if((ouiter = _outcomes.find(pt)) == _outcomes.end())
      return false;

   *out = ouiter->second;

   return true;
}

/********************************************************************************************/
/* Get the state and outcome value for the closest state to compareState found in _outcomes */
void team::getRecentOutcome(vector < double > &compareState, vector < double > &foundState, double *out, int distFunction)
{
   double minDist = HUGE_VAL;
   map < point *, double > :: iterator ouiter;
   vector <double> theirState;

   if (distFunction == 0){
      for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++){
	 (ouiter->first)->state(theirState);
	 double distTmp = EuclideanDistSqrdNorm(compareState, theirState);
	 if (distTmp < minDist){
	    (ouiter->first)->state(foundState);
	    *out = ouiter->second;
	    minDist = distTmp;
	 }
      }
   }
   else if (distFunction == 1){
      vector <int> compareStateInt;
      vector <int> theirStateInt;
      compareStateInt.insert(compareStateInt.end(),compareState.begin(),compareState.end());
      for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++){
	 (ouiter->first)->state(theirState);
	 theirStateInt.clear();
	 theirStateInt.insert(theirStateInt.end(),theirState.begin(),theirState.end());
	 double distTmp = normalizedCompressionDistance(compareStateInt, theirStateInt);
	 if (distTmp < minDist){
	    (ouiter->first)->state(foundState);
	    *out = ouiter->second;
	    minDist = distTmp;
	 }
      }
   }
}

/********************************************************************************************/
/* Calculate normalized compression distance w.r.t another team. */
double team::ncdBehaviouralDistance(team * t, int phase){
   double ncd;
   ostringstream oss;
   vector <int> theirBehaviourSequence;
   t->getBehaviourSequence(theirBehaviourSequence, phase);
   vector <int> myBehaviourSequence;
   getBehaviourSequence(myBehaviourSequence, phase);
   if (myBehaviourSequence.size() == 0 || theirBehaviourSequence.size() == 0)
      return -1;
   return normalizedCompressionDistance(myBehaviourSequence,theirBehaviourSequence);
}

/********************************************************************************************/
int team::numFrozen(){
   int frozen = 0;
   set < learner * > :: iterator leiter, leiterend;

   for(leiter = _members.begin(), leiterend = _members.end();
	 leiter != leiterend; leiter++)
   {
      if ((*leiter)->frozen() == true)
	 frozen++;
   }

   return frozen;
}

/********************************************************************************************/
long team::numOutcomes(int phase)
{
   long numOut = 0;
   map < point *, double > :: iterator ouiter;
   for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++)
      if (ouiter->first->phase() == phase)
	 numOut++;
   return numOut;
}

/********************************************************************************************/
void team::outcomes(int i, int phase, vector < double > &outcomes){
   map < point *, double > :: iterator ouiter;
   for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++)
      if ((ouiter->first)->phase() == phase)
	 outcomes.push_back((ouiter->first)->somedouble(i));
}

/********************************************************************************************/
void team::outcomes(map<point*,double,pointLexicalLessThan>&outcomes,int phase){
   outcomes = _outcomes;
   map < point *, double > :: iterator ouiter;
   for(ouiter = _outcomes.begin(); ouiter != _outcomes.end(); ouiter++)
      if ((ouiter->first)->phase() == phase)
	 outcomes.insert(*ouiter);
}

/********************************************************************************************/
string team::printBids(string prefix)
{
   set < learner * > :: iterator leiter;

   ostringstream oss, ossout;
   int i;

   for(i = 0, leiter = _members.begin(); leiter != _members.end(); i++, leiter++)
   {
      oss.str("");
      oss << prefix << " " << i << " lid " << (*leiter)->id() << " act " << (*leiter)->action();
      oss << " size " << (*leiter)->size() << " esize " << (*leiter)->esize();
      ossout << (*leiter)->printBid(oss.str());
   }

   return ossout.str();
}

/********************************************************************************************/
team::~team()
{
   set < learner * > :: iterator leiter, leiterend;

   for(leiter = _members.begin(), leiterend = _members.end();
	 leiter != leiterend; leiter++)
      (*leiter)->refDec();

   map < point *, double, pointLexicalLessThan > :: iterator ouiter;

   for (ouiter = _outcomes.begin(); ouiter != _outcomes.end();){
      delete ouiter->first;
      _outcomes.erase(ouiter++);
   }
}

/********************************************************************************************/
void team::removeLearner(learner *lr)
{
   set < learner * > :: iterator leiter;

   if((leiter = _members.find(lr)) == _members.end())
      die(__FILE__, __FUNCTION__, __LINE__, "should not remove learner that is not there");

   if (lr->atomic() || _level == 0)
      _nodes -= 1;
   else
      _nodes -= (*_actions)[lr->action()]->nodes();

   _members.erase(leiter);
   if((leiter = _active.find(lr)) != _active.end())
      _active.erase(leiter);
}

/********************************************************************************************/
void team::resetOutcomes(int phase)
{
   map < point *, double, pointLexicalLessThan > :: iterator ouiter;
   for (ouiter = _outcomes.begin(); ouiter != _outcomes.end();)
   {
      if ((ouiter->first)->phase() == phase)
	 _outcomes.erase(ouiter++);
      else
	 ++ouiter;
   }                
}

/********************************************************************************************/
void team::setMargin(point *pt,
      double margin)
{
   if((_margins.insert(map < point *, double >::value_type(pt, margin))).second == false)
      die(__FILE__, __FUNCTION__, __LINE__, "could not set margin, duplicate point?");
}

/********************************************************************************************/
void team::setOutcome(point *pt, double out, int nso)
{
   if((_outcomes.insert(map < point *, double >::value_type(pt, out))).second == false)
      die(__FILE__, __FUNCTION__, __LINE__, "could not set outcome, duplicate point?");

   if (numOutcomes(pt->phase()) > nso){
      map < point *, double, pointLexicalLessThan > :: iterator it;
      it = _outcomes.begin();
      while (it->first->phase() != pt->phase()) it++;
      _outcomes.erase(it);
   }
}

/********************************************************************************************/
void team::taskMap(long newDim)
{
   set < learner * > :: iterator leiter;

   for(leiter = _members.begin(); leiter != _members.end();leiter++)
      (*leiter)->taskMap(newDim);
}

/********************************************************************************************/
string team::toString(string prefix)
{
   set < learner * > :: iterator leiter;

   ostringstream oss;
   prefix = "   " + prefix;
   oss << prefix << " team::toString id " << _id;
   oss << " gtime " << _gtime;
   oss << " lev " << _level;
   oss << " nodes " << _nodes;
   oss << " numOut " << _outcomes.size();
   oss << " size " << _members.size();
   oss << " asize " << _active.size();
   oss << endl;

   oss << prefix << " learners";
   bool metaActionsPresent = false;
   for(leiter = _members.begin(); leiter != _members.end(); leiter++){
      vector < instruction * > bid;
      (*leiter)->getBid(bid);
      oss << " " << (*leiter)->id() << "(" << (long long)(*leiter) << ")->" << (*leiter)->action();
      //oss << " bid:";
      //for (int i = 0; i < bid.size(); i++) oss << " " << bid[i]->to_string();
      if ((*leiter)->atomic() == true)
	 oss << "(a)";
      else{
	 metaActionsPresent = true;
	 oss << "(m)";
      }
      if (_active.find((*leiter)) != _active.end()) oss << "A";
   }


   if (metaActionsPresent == true){ 
      oss << endl << "meta-actions:" << endl;
      for(leiter = _members.begin(); leiter != _members.end(); leiter++){
	 if ((*leiter)->atomic() == false){
	    oss << prefix << (*leiter)->id() <<"->" << (*leiter)->action() << ":" << endl;
	    oss << (*_actions)[(*leiter)->action()]->toString(prefix);
	 }
      }
   }
   else 
      oss << endl;
   return oss.str();
}

/********************************************************************************************/
void team::updateActiveMembersFromIds( vector < long > &activeMemberIds){
   sort(activeMemberIds.begin(), activeMemberIds.end()); //for binary search
   set < learner * > :: iterator leiter;
   for(leiter = _members.begin(); leiter != _members.end(); leiter++)
      if (binary_search(activeMemberIds.begin(), activeMemberIds.end(), (*leiter)->id()))
	 _active.insert(*leiter);
}
