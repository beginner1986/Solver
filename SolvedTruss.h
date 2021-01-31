#ifndef __CALCULATEDTRUSS_H__
#define __CALCULATEDTRUSS_H__

#include "Truss.h"

class SolvedTruss : public Truss
{
public:
    arma::Col<double> globalForces;
    arma::Col<double> globalDisplacements;
    arma::Col<double> reactionForces;
    std::vector<arma::Col<double>> elementsInternalStress;

public:
    SolvedTruss(Truss *truss);
    void solve();
};

#endif // __CALCULATEDTRUSS_H__
