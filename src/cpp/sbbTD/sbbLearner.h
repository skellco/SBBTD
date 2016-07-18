#ifndef sbbLearner_h
#define sbbLearner_h

#include <bitset>
#include <cmath>
#include <map>
#include "sbbMisc.h"
#include <set>
#include <vector>

#define INPUTS 65536 /* Requires 16 bits. */
#define REGISTERS 8 /* Should match _dstMask. */
/* Used to define unique bidding behaviour. */
#define PROFILE_SIZE_INI 50 /* Compare this many bid values. */
#define PROFILE_SIZE 50 /* Compare this many bid values. */
#define BID_EPSILON 1e-5 /* Using this equality threshold. */

using namespace std;

typedef bitset < 16 + 3 + 3 + 1 > instruction;

class learner
{
	private:
		long _action; /* Action index. */
		bool _atomic; /* If true, always interpret this learner's action as atomis. */
		long _ancestral_gtime;
		vector < instruction * > _bid; /* Bid program. */
		static long _count; /* Next id to use. */
		long _dim; /* Expected dimension of input feature vector. */
		int _esize; /* Size of program not counting structural introns identified by markIntrons(). */
		set < long > _features; /* Features indexed by non-introns in this learner, determined in markIntrons(). */
		bool _frozen; /* If true, never modify this learner. */
		long _gtime; /* Time step at which generated. */
		unsigned long _id; /* Unique id of learner. */
		/* Mark structural introns (set to TRUE if NOT an intron).
		   For example, _introns[0] is TRUE if the first instruction is effective. */
		vector < bool > _introns;
		double _key;
                int _lastCompareFactor;
		bool _lifer; /* If true, never remove this learner from a team. */
		int _nrefs; /* Number of references by teams. */
		vector < double > _profile; /* Bid profile. */

		/* Instruction definitions. */

		/* Masks. Apply before shifting. */
		static const instruction _modeMask;
		static const instruction _opMask;
		static const instruction _dstMask;
		static const instruction _srcMask;

		/* Modes. */
		static const instruction _mode0;
		static const instruction _mode1;

		/* Operations. */
		static const instruction _opSum;
		static const instruction _opDiff;
		static const instruction _opProd;
		static const instruction _opDiv;
		static const instruction _opCos;
		static const instruction _opLog;
		static const instruction _opExp;
		static const instruction _opCond;

		/* Shift amounts. */
		static const short _modeShift;
		static const short _opShift;
		static const short _dstShift;
		static const short _srcShift;

		void markIntrons(vector < instruction * > &);
		void mutateFeatures(vector < instruction * > &);
		double run(vector < instruction * > &, double *, double *);
		void setFeature (instruction &, ulong feat);
	public:
		inline long action() { return _action; }
		inline void action(long a) { _action = a; }
		inline long ancestralGtime() { return _ancestral_gtime; }
		inline bool atomic() { return _atomic; }
		inline void atomic(bool a) { _atomic = a; }
		double bid(double *, double *);
		string checkpoint();
		inline long dim() { return _dim; }
		inline void dim(long d) { _dim = d; }
		inline int esize() { return _esize; } /* Not counting introns. */
		inline void features(set < long > &F) { F.insert(_features.begin(), _features.end()); }
		inline bool frozen() { return _frozen; }
		inline void frozen(bool f) { _frozen = f; }
		inline void getBid(vector <instruction *> &b) { b = _bid;}
		inline void getProfile(vector < double > &p) { p = _profile; }
		inline long gtime() { return _gtime; }
		inline unsigned long id() { return _id; }
                inline unsigned long id(long i) { _id = i; }
		inline bool isProfileEqualTo(vector < double > &p) { return isEqual(_profile, p, BID_EPSILON); }
		inline double key(){ return _key; }
		inline void key(double key) { _key = key; }
                inline int lastCompareFactor() { return _lastCompareFactor; }
                inline void lastCompareFactor(int c) { _lastCompareFactor = c; }
		learner(long, long, int, long, long); /* Create arbitrary learner. */
		learner(long, learner &, long); /* Create learner from another learner. */
		learner(long, long, long, long, long, long, vector < instruction * >); /* Create learner from checkpoint file. */
		~learner();
		inline bool lifer() { return _lifer; }
		inline void lifer(bool l) { _lifer = l; }
		/* Mutate action, return true if the action was actually changed. */
		inline bool muAction(long action) { long a = _action; _action = action; return a != action; }
		bool muBid(double, double, double, double, int); /* Mutate bid, return true if any changes occured. */
		inline long numFeatures() { return _features.size(); } /* Not counting introns. */
		string printBid(string);
		inline void setId(long id) { _id = id; }
		inline void setNrefs(int nrefs) { _nrefs = nrefs; }
		inline void setProfile(vector < double > &p) { _profile = p; }
		inline int size() { return _bid.size(); }
		inline void taskMap(long newDim){ _dim = newDim; mutateFeatures(_bid); }
		inline int refs() { return _nrefs; }
		inline int refDec() { return --_nrefs; }
		inline int refInc() { return ++_nrefs; }

		friend ostream & operator<<(ostream &, const learner &);

};
struct LearnerBidLexicalCompare {
	bool operator()(learner* l1, learner* l2) { 
		if (l1->key() != l2->key()){ l1->lastCompareFactor(0); l2->lastCompareFactor(0); return l1->key() > l2->key();} //bid, higher is better
		else if (l1->esize() != l2->esize()){ l1->lastCompareFactor(1); l2->lastCompareFactor(1);  return l1->esize() < l2->esize();} //program size post intron removal, smaller is better (assumes markIntrons is up to date)
		else if (l1->refs() != l2->refs()){ l1->lastCompareFactor(2); l2->lastCompareFactor(2); return l1->refs() < l2->refs();} //number of references, less is better
		else if (l1->numFeatures() != l2->numFeatures()){ l1->lastCompareFactor(3); l2->lastCompareFactor(3); return l1->numFeatures() < l2->numFeatures();} //number of features indexed, less is better
		else if (l1->gtime() != l2->gtime()){ l1->lastCompareFactor(4); l2->lastCompareFactor(4); return l1->gtime() > l2->gtime();} //age, younger is better
		else if (l1->ancestralGtime() != l2->ancestralGtime()){ l1->lastCompareFactor(5); l2->lastCompareFactor(5); return l1->ancestralGtime() > l2->ancestralGtime();} //age of oldest ancestor, younger is better
		else { l1->lastCompareFactor(6); l2->lastCompareFactor(6); return l1->id() < l2->id();} //correlated to age but technically arbirary, id is guaranteed to be unique and thus ensures deterministic comparison
	}
};
#endif
