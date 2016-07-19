

#include "FuncApprox.h"

FunctionApproximator::FunctionApproximator( int numF, int numA, 
					    double r[],
					    double m[],
					    double res[] )
{
  numFeatures = numF;
  numActions  = numA;

  for ( int i = 0; i < numFeatures; i++ ) {
    ranges     [ i ] = r  [ i ];
    minValues  [ i ] = m  [ i ];
    resolutions[ i ] = res[ i ];
  }
  
  /////shiva start
  //for the sake of the neural net
  //must cover the only value: 1
    ranges[numFeatures] = 1.0;
    minValues[numFeatures] = 0.0;
    resolutions[numFeatures] = 1.0;
    
  /////////shiva end
  
  
  
}


void FunctionApproximator::setState( double s[] )
{
  for ( int i = 0; i < numFeatures; i++ ) {
    state[ i ] = s[ i ];
  }

    ////////shiva start
    state[numFeatures] = 1.0; 
////////shiva end

}
