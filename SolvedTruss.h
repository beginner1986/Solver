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
    Truss &truss;
    double displacementsScale;

public:
    SolvedTruss(Truss &truss);
    void solve();
    void draw(std::string fileName);
    Truss getInputTruss() const { return truss; }

private:
    double calculateScale();
    void applyDisplacements(arma::Col<double> displacements);
};

#endif // __CALCULATEDTRUSS_H__
