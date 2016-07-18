#include "sbbPoint.h"

/********************************************************************************************/
string point::checkPoint(){
   ostringstream oss;

   oss <<"point:" << _id << ":" << _gtime << ":" << _label << ":" << _someint;
   for (int i = 0; i <_state.size(); i++)
      oss << ":" << _state[i];
   oss << endl;

   return oss.str();
}

/********************************************************************************************/
bool point::isPointUnique(set < point * > &P)
{
   vector < double > state;
   set < point * > :: iterator poiter;

   for(poiter = P.begin(); poiter != P.end(); poiter++)
   {
      (*poiter)->state(state);

      if(isEqual(_state, state, 1e-5))
         return false;
   }

   return true;
}

/********************************************************************************************/
point::point(long id, 
      int dim,
      long gtime,
      const double *state,
      long label)
: _id(id), _gtime(gtime), _label(label), _key(0), _marked(0), _solved(0), _slice(0), _someint(0)
{
   _state.reserve(dim);
   _state.resize(dim);

   for(int i = 0; i < dim; i++)
      _state[i] = state[i];
}

/********************************************************************************************/
// this version used for checkpointing
point::point(long id,
      int dim,
      long gtime,
      const double *state,
      long label,
      int someint)
: _id(id), _gtime(gtime), _label(label), _someint(someint),_key(0), _marked(0), _solved(0), _slice(0)
{
   _state.reserve(dim);
   _state.resize(dim);

   for(int i = 0; i < dim; i++)
      _state[i] = state[i];
}

/********************************************************************************************/
point::point(long id,
      int dim,
      long gtime,
      const double *state,
      long label,
      double somedouble_1,
      double somedouble_2,
      double somedouble_3,
      int phase)
: _id(id), _gtime(gtime), _label(label), _someint(0), _somedouble_1(somedouble_1), _somedouble_2(somedouble_2), _somedouble_3(somedouble_3), _phase(phase), _key(0), _marked(0), _solved(0), _slice(0)
{
   _state.reserve(dim);
   _state.resize(dim);

   for(int i = 0; i < dim; i++)
      _state[i] = state[i];
}

/********************************************************************************************/
ostream & operator<<(ostream &os, const point &pt)
{
   os << "(" << pt._id << ", " << pt._label << ", " << pt._gtime << ")";
   return os;
}
