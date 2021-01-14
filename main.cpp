#include <iostream>
#include <vector>
#include <armadillo>

#include "Truss.h"
#include "Solver.h"

int main()
{
    std::cout << "Truss initialization..." << std::endl;
    Truss truss(1, 1);

    std::cout << "Solving the truss..." << std::endl;
    Solver solver(truss);
    solver.solve();

    arma::Col<double> globalForces = solver.getGlobalForces();
    arma::Col<double> globalDisplacements = solver.getGlobalDisplacements();
    arma::Col<double> reactionForces = solver.getReactionForces();
    std::vector<arma::Col<double> > elementsInternalStress = solver.getElementsInternalStress();

    std::cout << "RESULTS" << std::endl;
    std::cout << "Global forces vector:" << std::endl;
    std::cout << globalForces << std::endl;
    std::cout << "Global displacements vector:" << std::endl;
    std::cout << globalDisplacements << std::endl;
    std::cout << "Reaction forces vector:" << std::endl;
    std::cout << reactionForces << std::endl;
    std::cout << "Elements internal stress vectors:" << std::endl;
    for(size_t i=0; i<elementsInternalStress.size(); i++)
    {
        std::cout << "\tElement " << i << std::endl;
        std::cout << elementsInternalStress.at(i) << std::endl;
    }

    return 0;
}
