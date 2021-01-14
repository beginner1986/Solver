#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <vector>

#include "Truss.h"

class Solver
{
private:
    const Truss& truss;
    std::vector<double> sins;
    std::vector<double> coss;
    std::vector<double> lengths;
    std::vector<arma::Mat<double> > globalElementsStiffness;
    arma::Mat<double> globalStiffnessMatrix;
    arma::Col<double> globalForces;
    arma::Col<double> globalDisplacements;
    arma::Col<double> reactionForces;
    std::vector<arma::Col<double> > elementsInternalStress;

public:
    Solver(const Truss& truss);
    arma::Col<double> calculateGlobalDisplacements(const arma::Mat<double>& globalStiffnessMatrix, arma::Col<double>& globalForces);
    arma::Col<double> calculateReactionForces(std::vector<arma::Col<double> >& globalInternalForces);
    std::vector<arma::Col<double> > calculateElementsInternalStress(std::vector<arma::Col<double> >& globalInternalForces);

private:
    std::tuple<std::vector<double>, std::vector<double>, std::vector<double> > calculateSinCosLen();
    std::vector<arma::Mat<double> > calculateElementsStiffnessGlobal();
    arma::Mat<double> calculateGlobalStiffnessMatrix();
    std::vector<arma::Col<double> > calculateGlobalInternalForces();
};

#endif // __SOLVER_H__