#include "Truss.h"

Truss::Truss(double A, double E) : A(A), E(E)
{
    coordinates = { 
        0, 0,       // node 0
        100, 0,     // node 1
        100, 100,   // node 2
    };

    dofsCount = coordinates.size();
    nodesCount = dofsCount / 2;

    topology = {
        { 0, 1, 2, 3 },     // node 1 to 1
        { 2, 3, 4, 5 },     // node 1 to 2
        { 0, 1, 4, 5 },     // node 0 to 2
    };
    elementsCount = topology.n_rows;

    constrains = { 
        true, true,     // node 0 fixed along x and y axis
        true, true,     // node 1 fixed along x and y axis
        false, false,   // node 2 not fixed along x and y axis
    };

    externalForces = { 
        0, 0,           // no forces at node 0
        0, 0,           // no forces at node 1
        2, -1,          // no forces at node 2
    };
}