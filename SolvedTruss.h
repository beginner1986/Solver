#ifndef __CALCULATEDTRUSS_H__
#define __CALCULATEDTRUSS_H__

#include "Truss.h"
#include "Solver.h"

class SolvedTruss : public Truss
{
public:
    arma::Col<double> globalForces;
    arma::Col<double> globalDisplacements;
    std::vector<double> reactionForces;
    std::vector<arma::Col<double>> elementsInternalStress;

private:
    Truss &truss;

public:
    SolvedTruss(Truss &truss);
    void solve(SOLVER_OPTS opts);
    void draw(std::string fileName);
    Truss getInputTruss() const { return truss; }

private:
    void applyDisplacements(arma::Col<double> displacements);
};

#endif // __CALCULATEDTRUSS_H__
