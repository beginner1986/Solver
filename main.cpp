#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <armadillo>

#include "Truss.h"
#include "Solver.h"

int main()
{
    Truss truss(1, 1);

    Solver solver(truss);

    // reaction forces caused by the constrains
    arma::Col<double> reactions(truss.dofsCount, arma::fill::zeros);
    std::vector<arma::Col<double> > internalForcesGlobalVectors;

    // elements internal forces
    for(size_t element=0; element<truss.elementsCount; element++)
    {
        // degrees of freedom
        std::array<uint, 4> dofs = { 
            truss.topology(element, 0), 
            truss.topology(element, 1), 
            truss.topology(element, 2), 
            truss.topology(element, 3) 
        };

        // element's displacements in global CS
        arma::Col<double> elementDisplacements(4);
        for(size_t j=0; j<4; j++)
        {
            elementDisplacements(j) = dispalcementsGlobal(dofs[j]);
        }

        // elements' internal forces (stress)
        arma::Col<double> internalForcesGlobal(4);
        internalForcesGlobal = elementsStiffnessGlobalMatrices.at(element) * elementDisplacements;
        internalForcesGlobalVectors.push_back(internalForcesGlobal);

        // if current dof is fixed, then include it's value into the reactions vector
        for(size_t j=0; j<4; j++)
        {
            if(truss.constrains.at(dofs[j]))
                reactions(dofs[j]) += internalForcesGlobal(j);
        }

        std::cout << "ELEMENT " << element << " internal forces vector: " << std::endl;
        std::cout << internalForcesGlobal << std::endl;
    }

    std::cout << "Reaction forces: " << std::endl;
    std::cout << reactions << std::endl;

    // internal forces in local CS
    std::vector<arma::Col<double> > internalForcesLocalVectors;

    std::cout << "Internal forces in local coordinate systems:" << std::endl;
    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        arma::Col<double> internalForcesLocal(4);

        arma::Mat<double> transformationMatrix = {
            { coss[element], sins[element], 0, 0 },
            { -sins[element], coss[element], 0, 0 },
            { 0, 0, coss[element], sins[element] },
            { 0, 0, -sins[element], coss[element] }
        };
        
        solve(internalForcesLocal, transformationMatrix, internalForcesGlobalVectors.at(element));
        internalForcesLocalVectors.push_back(internalForcesLocal);

        std::cout << "\tELEMENT " << element << " :" << std::endl;
        std::cout << internalForcesLocal << std::endl;
    }

    return 0;
}
