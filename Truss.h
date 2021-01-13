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
    size_t dofsCount;

    // nodes connections by the elements
    arma::Mat<uint> topology;
    size_t elementsCount;

    // constrains of each node and x and y directions (true=fixed, false=not fixed)
    std::vector<bool> constrains;

    // external forces applied to the truss
    std::vector<double> externalForces;

    Truss(double A, double E);
};

#endif // __TRUSS_H__
