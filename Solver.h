#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <iostream>
#include <vector>
#include <armadillo>

#include "Truss.h"

enum class SOLVER_OPTS {
    DENSE,
    SPARSE
};

class Solver
{
private:
    const Truss &truss;
    std::vector<double> sins; // sin of angle between elements' local CS and global CS
    std::vector<double> coss; // cos of angle between elements' local CS and global CS
    std::vector<double> lengths;
    std::vector<arma::Mat<double>> globalElementsStiffness;
    arma::Col<double> globalForces;
    arma::Col<double> globalDisplacements;
    std::vector<double> reactionForces;
    std::vector<arma::Col<double>> elementsInternalStress;

public:
    Solver(const Truss &truss) : truss(truss) {}

    void solve(SOLVER_OPTS opts);
    arma::Col<double> getGlobalForces() const { return globalForces; }
    arma::Col<double> getGlobalDisplacements() const { return globalDisplacements; }
    std::vector<double> getReactionForces() const { return reactionForces; }
    std::vector<arma::Col<double>> getElementsInternalStress() const { return elementsInternalStress; }

private:
    std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> calculateSinCosLen();
    std::vector<arma::Mat<double>> calculateElementsStiffnessGlobal();
    arma::Mat<double> calculateGlobalStiffnessMatrix();
    arma::SpMat<double> calculateGlobalStiffnessSparseMatrix();
    std::vector<arma::Col<double>> calculateGlobalInternalForces();

    arma::Col<double> calculateGlobalDisplacements(const arma::Mat<double> &globalStiffnessMatrix, arma::Col<double> &globalForces);
    arma::Col<double> calculateGlobalDisplacements(const arma::SpMat<double> &globalStiffnessMatrix, arma::Col<double> &globalForces);
    std::vector<double> calculateReactionForces(std::vector<arma::Col<double>> &globalInternalForces);
    std::vector<arma::Col<double>> calculateElementsInternalStress(std::vector<arma::Col<double>> &globalInternalForces);
};

#endif // __SOLVER_H__