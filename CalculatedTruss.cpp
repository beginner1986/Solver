#include "CalculatedTruss.h"

CalculatedTruss::CalculatedTruss(const Truss *truss)
{
    this->A = truss->A;
    this->E = truss->E;
    this->coordinates = truss->coordinates;
    this->dofsCount = truss->dofsCount;
    this->nodesCount = truss->nodesCount;
    this->topology = truss->topology;
    this->elementsCount = truss->elementsCount;
    this->constrains = truss->constrains;
    this->externalForces = truss->externalForces;
}
