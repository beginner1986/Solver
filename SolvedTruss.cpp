#include <cmath>
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

    displacementsScale = calculateScale();
    applyDisplacements(globalDisplacements);
}

void SolvedTruss::draw(std::string fileName)
{
    Drawer trussDrawer(*this, fileName);
    trussDrawer.draw(*this);
}

double SolvedTruss::calculateScale()
{
    double maxDisplacement = 0;
    for (double d : globalDisplacements)
    {
        maxDisplacement = std::max(maxDisplacement, std::abs(d));
    }

    double maxCoordinate = 0;
    for (double c : coordinates)
    {
        maxCoordinate = std::max(maxCoordinate, std::abs(c));
    }

    double scale = maxCoordinate / maxDisplacement / 10;

    return scale;
}

void SolvedTruss::applyDisplacements(arma::Col<double> displacements)
{
    for (size_t c = 0; c < dofsCount; c++)
    {
        coordinates.at(c) += (displacements.at(c) * displacementsScale);
    }
}
