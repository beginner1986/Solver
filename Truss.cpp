#include "Truss.h"

Truss::Truss(double A, double E, std::vector<double> coordinates,
             arma::Mat<uint> topology, std::vector<bool> constrains,
             std::vector<double> externalForces)
    : A(A), E(E), coordinates(coordinates), topology(topology), constrains(constrains), externalForces(externalForces)
{
    dofsCount = coordinates.size();
    nodesCount = dofsCount / 2;
    elementsCount = topology.n_rows;
}
