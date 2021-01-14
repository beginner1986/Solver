#ifndef __TRUSS_H__
#define __TRUSS_H__

#include <vector>
#include <armadillo>

struct Truss
{
public:
    double A;       // elements' cross area
    double E;       // Young's modulus
    
    std::vector<double> coordinates;    // nodes coordinates definition (each node x and y)
    size_t dofsCount;                   // degrees of freedom number   

    arma::Mat<uint> topology;           // nodes connections by the elements
    size_t elementsCount;

    std::vector<bool> constrains;       // if true then dof is fixed else it's not fixed

    std::vector<double> externalForces; // external forces applied to the truss

    Truss(double A, double E);
};

#endif // __TRUSS_H__
