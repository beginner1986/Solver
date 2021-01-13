#ifndef __TRUSS_H__
#define __TRUSS_H__

#include <vector>
#include <armadillo>

class Truss
{
    public:
    // material properties
    double A;       // elements' cross area
    double E;       // Young's modulus

    // nodes coordinates definition (each node x and y) in one table as they are also dofs
    std::vector<double> coordinates;
    size_t nodesCount;
    size_t dofsCount;

    // nodes connections by the elements
    size_t elementsCount;
    arma::Mat<uint> topology;

    // constrains of each node and x and y directions (true=fixed, false=not fixed)
    std::vector<bool> constrains;

    // external forces applied to the truss
    std::vector<double> externalForces;

    Truss(double A = 1, double E = 1) : A(A), E(E)
    {
        coordinates= { 
            0, 0,       // node 0
            1, 0,       // node 1
            1, 1,       // node 2
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
};

#endif // __TRUSS_H__
