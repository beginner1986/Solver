#include <armadillo>

#include "SolvedTruss.h"
#include "Solver.h"

SolvedTruss::SolvedTruss(Truss *truss)
    : Truss(truss->A, truss->E, truss->coordinates, truss-> topology, truss->constrains, truss->externalForces)
{
}

void SolvedTruss::solve()
{
    Solver solver(*this);
    solver.solve();

    this->globalForces = solver.getGlobalForces();
    this->globalDisplacements = solver.getGlobalDisplacements();
    this->reactionForces = solver.getReactionForces();
    this->elementsInternalStress = solver.getElementsInternalStress();
}
