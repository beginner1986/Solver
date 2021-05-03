#include <cmath>
#include <armadillo>

#include "SolvedTruss.h"
#include "Drawer.h"

SolvedTruss::SolvedTruss(Truss &truss)
    : Truss(truss.A, truss.E, truss.coordinates, truss.topology, truss.constrains, truss.externalForces),
      truss(truss)
{
}

void SolvedTruss::solve(SOLVER_OPTS opts)
{
    Solver solver(*this);
    solver.solve(opts);

    this->globalForces = solver.getGlobalForces();
    this->globalDisplacements = solver.getGlobalDisplacements();
    this->reactionForces = solver.getReactionForces();
    this->elementsInternalStress = solver.getElementsInternalStress();

    applyDisplacements(globalDisplacements);
}

void SolvedTruss::draw(std::string fileName)
{
    Drawer trussDrawer(*this, fileName);
    trussDrawer.draw(*this);
}

void SolvedTruss::applyDisplacements(arma::Col<double> displacements)
{
    for (size_t c = 0; c < dofsCount; c++)
    {
        coordinates.at(c) += displacements.at(c);
    }
}
