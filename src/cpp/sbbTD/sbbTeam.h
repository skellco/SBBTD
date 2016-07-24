#ifndef team_h
#define team_h

#include <map>
#include "sbbLearner.h"
#include "sbbMisc.h"
#include "sbbPoint.h"
#include <set>

using namespace std;

class team
{
   /* If the team is at level 0, the action of each learner corresponds to an actionin the environment. 
      If the team is at any other level, the action of each learner is an index into _actions. */
   vector < team * > *_actions; /* Teams in the level directly below. */
   set < learner * > _active; /* Active member learners, a subset of _members, activated in getAction(). */
   vector <long> _ancestors; /* lineage */
   bool _archived;
   static long _count; /* Next id to use. */
   multiset <double> _distances_0;
   multiset <double> _distances_1;
   int _domOf; /* Number of other teams that this team dominates. */
   int _domBy; /* Number of other teams dominating this team. */
   double _fit;	
   long _gtime; /* Time step at which generated. */
   long _id; /* Unique id of team. */
   double _key; /* For sorting. */
   int _lastCompareFactor;
   long _level;/* The level the team is at. */
   map < point *, double > _margins; /* Maps point->margin. */
   set < learner * > _members; /* The member learners. */
   int _nodes; /* Number of nodes at all levels assuming this team is the root. */
   map < point *, double, pointLexicalLessThan > _outcomes; /* Maps point->outcome. */
   double _parentFitness;
   double _parentNovelty;
   double _parentScore;
   double _score;
   long _tmpNumOutcomes;

   public:
   inline void activeMembers(set < learner * > *m) { m->insert(_active.begin(), _active.end()); }
   inline void addAncestor(long a) { _ancestors.push_back(a); }
   inline void addDistance(int type, double d){ if (type == 0) _distances_0.insert(d); else if (type == 1) _distances_1.insert(d);}
   bool addLearner(learner *);
   inline double archived(){ return _archived; }
   inline void archived(bool a) { _archived = a; }
   inline int asize() const { return _active.size(); } /* The number of active learners in this team. */
   string checkpoint(int);
   inline void clearDistances(){ _distances_0.clear(); _distances_1.clear();}
   long collectiveAge(long);
   void deleteMargin(point *); /* Delete margin. */
   void deleteOutcome(point *); /* Delete outcome. */
   inline int domBy() {return _domBy;}
   inline void domBy(int d){_domBy = d;}
   inline int domOf() {return _domOf;}
   inline void domOf(int d){_domOf = d;}
   void features(set < long > &) const;
   double symbiontUtilityDistance(team*, long);
   double symbiontUtilityDistance(vector < long > &, long);
   long getAction(vector < double > &, vector < learner * > &, vector < double > &, vector < double > &, bool);
   long getAction(vector < double > &, bool, vector < set <learner *, LearnerBidLexicalCompare> > &, long &, vector <long> &);
   inline void getAllAncestors(vector <long> &a) { a = _ancestors; }
   void getBehaviourSequence(vector <int> &, int);
   inline double fit(){ return _fit; }
   inline void fit(double f) { _fit = f; }
   bool getMargin(point *, double *); /* Get margin, return true if found. */
   double getMaxOutcome(int,int);
   void getMeanActionProfileAndMeanReward(vector < double >&, double *, int);
   double getMeanOutcome(int,int,double);
   double getMedianOutcome(int,int);
   double getMinOutcome(int,int);
   /* Get the behaviourProfile and reward for the closest  behaviourProfile in _outcomes */
   void getMostSimilarBehaviouralOutcome(vector < double >&, double *, double *);
   bool getOutcome(point *, double *); /* Get outcome, return true if found. */
   /* Get the state and outcome value for the colsest state in _outcomes (endState */
   void getRecentOutcome(vector < double >&,vector < double >&, double *, int);
   inline long gtime() const { return _gtime; }
   inline long id() const { return _id; }
   inline void id(long id) { _id = id; }
   inline double key() { return _key; }
   inline void key(double key) { _key = key; }
   inline int lastCompareFactor() { return _lastCompareFactor; }
   inline void lastCompareFactor(int c) { _lastCompareFactor = c; }
   inline long level() { return _level; }
   inline void members(set < learner * > *m) { m->insert(_members.begin(), _members.end()); }
   double ncdBehaviouralDistance(team*, int);
   inline int nodes() const { return _nodes; } /* This is the number of nodes at all levels. */
   double nov(int, int);
   //inline void nov(int type, double n) { if (type==0) _nov_0 = n; else if (type==1) _nov_1 = n; }
   int numFrozen();
   inline long numMargins() { return _margins.size(); } /* Number of margins. */
   long numOutcomes(int phase); /* Number of outcomes. */
   void outcomes(map<point*, double, pointLexicalLessThan>&, int); /* Get all outcomes from a particular phase */
   void outcomes(int, int, vector < double >&); /* Get all outcome values of a particular type and from a particular phase.*/
   inline double parentFitness(){ return _parentFitness; }
   inline void parentFitness(double f) { _parentFitness = f; }
   inline double parentNovelty(){ return _parentNovelty; }
   inline void parentNovelty(double n) { _parentNovelty = n; }
   inline double parentScore(){ return _parentScore; }
   inline void parentScore(double n) { _parentScore = n; }
   int policyFeatures(set < long > &, bool); //populates set with features and returns number of nodes(learners) in policy
   int policyInstructions(bool);
   int policyHosts(vector<long> &, int, bool);
   string printBids(string);
   void removeLearner(learner *);
   void resetOutcomes(int); /* Delete all outcomes from phase. */
   void setMargin(point *, double); /* Set margin. */
   inline double score(){ return _score; }
   inline void score(double f) { _score = f; }
   void setOutcome(point *, double, int nso); /* Set outcome. */
   inline int size() const { return _members.size(); } /* This is the team size at the current level. */
   void taskMap(long newDim);
   team(long level, vector < team * > *actions, long gtime): _level(level), _actions(actions), _id(_count++), _gtime(gtime), _key(0), _nodes(1) {
      _fit = 0.0; _parentFitness = 0.0; _parentNovelty = 0.0; _parentScore = 0.0; _score = 0; _archived = false;
   };
   //this constructor used for checkpointing
   team(long level, vector < team * > *actions, long gtime, long id): _level(level), _actions(actions), _id(id), _gtime(gtime),  _key(0), _nodes(1) {
      _fit = 0.0; _parentFitness = 0.0; _parentNovelty = 0.0; _parentScore = 0.0; _score = 0; _archived = false;
   };
   ~team(); /* Affects learner refs, unlike addLearner() and removeLearner(). */
   inline long tmpNumOutcomes() { return _tmpNumOutcomes; }
   inline void tmpNumOutcomes(long i) {_tmpNumOutcomes = i; }
   string toString(string);
   void updateActiveMembersFromIds( vector < long > &);

   friend ostream & operator<<(ostream &, const team &);  
};

struct teamIdComp
{
   bool operator() (team* t1, team* t2) const
   {
      return t1->id() < t2->id();
   }
};

struct teamScoreLexicalCompare {
   bool operator()(team* t1, team* t2) {
      set < long > policyFeaturesT1;
      int pf_t1 = t1->policyFeatures(policyFeaturesT1,false);
      set < long > policyFeaturesT2;
      int pf_t2 = t2->policyFeatures(policyFeaturesT2,false);
      if (t1->score() != t2->score()){ t1->lastCompareFactor(0); t2->lastCompareFactor(0); return t1->score() > t2->score();} //score, higher is better
      else if (t1->asize() != t2->asize()){ t1->lastCompareFactor(1); t2->lastCompareFactor(1);  return t1->asize() < t2->asize();} //team size post intron removal (active members), smaller is better
      else if (pf_t1 != pf_t2){ t1->lastCompareFactor(3); t2->lastCompareFactor(3); return pf_t1 < pf_t2;} //total number of learners (nodes), less is better
      else if (policyFeaturesT1.size()  != policyFeaturesT2.size()){ t1->lastCompareFactor(4); t2->lastCompareFactor(4); return policyFeaturesT1.size() < policyFeaturesT2.size();} //total number of features, less is better
      else if (t1->gtime() != t2->gtime()){ t1->lastCompareFactor(5); t2->lastCompareFactor(5); return t1->gtime() > t2->gtime();} //age, younger is better
      else { t1->lastCompareFactor(6); t2->lastCompareFactor(6); return t1->id() < t2->id();} //correlated to age but technically arbirary, id is guaranteed to be unique and thus ensures deterministic comparison
   }
};


#endif
