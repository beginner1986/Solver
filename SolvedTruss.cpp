#include <armadillo>

#include "SolvedTruss.h"
#include "Solver.h"
#include "Drawer.h"

SolvedTruss::SolvedTruss(Truss &truss)
    : Truss(truss.A, truss.E, truss.coordinates, truss.topology, truss.constrains, truss.externalForces),
    truss(truss)
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

    implementDisplacements();
}

void SolvedTruss::draw(std::string fileName) 
{
    Drawer trussDrawer(truss, fileName);
    trussDrawer.draw();
}

void SolvedTruss::implementDisplacements() 
{
    for(size_t c=0; c<dofsCount; c++)
    {
        coordinates.at(c) = coordinates.at(c) + globalDisplacements.at(c) * displacementsScale;
    }
}
