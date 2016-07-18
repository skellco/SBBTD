#ifndef sbbpoint_h
#define sbbpoint_h

#include <vector>
#include <map>
#include <set>

#include "sbbMisc.h"
using namespace std;

class point
{
   long _gtime;
   long _id;
   double _key; /* For sorting. */
   long _label;
   bool _marked;
   int _phase; /* Phase: TRAIN_MODE, VALIDATION_MODE, or TEST_MODE. */
   double _slice; /* For roulette wheel. */
   double _somedouble_1; /* Some double, can be used by the environment as required. */
   double _somedouble_2; /* Some double, can be used by the environment as required. */
   double _somedouble_3; /* Some double, can be used by the environment as required. */
   int _someint; /* Some integer, can be used by the environment as required. */
   bool _solved; /* If this point has been solved at some time. */
   vector < double > _state;

   public:
   string checkPoint();
   inline int dim(){ return _state.size(); }
   inline long gtime(){ return _gtime; }
   inline long id(){ return _id; }
   bool isPointUnique(set < point * > &); /* Return true if the point is unique w.r.t. another set. */
   inline bool isPointUnique(vector < point * > &P)
   { set < point * > PS; PS.insert(P.begin(), P.end()); return isPointUnique(PS); }
   inline double key(){ return _key; }
   inline void key(double key){ _key = key; }
   inline long label(){ return _label; }
   inline void mark(){ _marked = 1; }
   inline bool marked(){ return _marked; }
   inline int phase(){ return _phase; }
   inline void phase(int p){ _phase = p; }
   point(long, int, long, const double *, long);
   point(long, int, long, const double *, long, int); //used for checkpointing
   point(long, int, long, const double *, long, double, double, double,int); 
   inline void setState(vector <double> s){ _state = s; }
   inline double slice(){ return _slice; }
   inline void slice(double slice){ _slice = slice; }
   inline bool solved(){ return _solved; }
   inline void solved(bool s){ _solved = s; }
   inline double somedouble(int i)
   { 
      if (i == 1) return _somedouble_1; 
      else if (i == 2) return _somedouble_2;
      else if (i == 3) return _somedouble_3;
   }
   inline void somedouble(int i, double d)
   { 
      if (i == 1) _somedouble_1 = d; 
      else if (i == 2) _somedouble_2 = d;
      else if (i == 3) _somedouble_3 = d;
   }
   inline int someint(){ return _someint; }
   inline void someint(int i){ _someint = i; }
   inline void state(vector < double > &v){ v = _state; }
   friend ostream & operator<<(ostream &, const point &);
};

struct pointLexicalLessThan {
   bool operator()( point* p1 , point* p2) const
   {
      if (p1->somedouble(TRAIN_REWARD) != p2->somedouble(TRAIN_REWARD)){ return p1->somedouble(TRAIN_REWARD) < p2->somedouble(TRAIN_REWARD); } //reward, smaller implies less
      else if (p1->gtime() != p2->gtime()){return p1->gtime() < p2->gtime();} //age, older implies less
      else return  p1->id() < p2->id(); //correlated to age but technically arbirary, id is guaranteed to be unique and thus ensures deterministic comparison
   }
};
#endif
