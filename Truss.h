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
    size_t nodesCount;

    arma::Mat<uint> topology;           // nodes connections by the elements
    size_t elementsCount;

    std::vector<bool> constrains;       // if true then dof is fixed else it's not fixed

    std::vector<double> externalForces; // external forces applied to the truss

    Truss() {}
    Truss(double A, double E, std::vector<double> coordinates, 
        arma::Mat<uint> topology, std::vector<bool> constrains, 
        std::vector<double> externalForces
    );
};

#endif // __TRUSS_H__
