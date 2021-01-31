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

private:
    Truss *truss;
    const double displacementsScale = 1e4;

public:
    SolvedTruss(Truss *truss);
    void solve();

private:
    void implementDisplacements();
};

#endif // __CALCULATEDTRUSS_H__
