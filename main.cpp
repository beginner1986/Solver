#include <iostream>
#include <vector>
#include <armadillo>

#include "FileReader.h"
#include "Truss.h"
#include "Solver.h"
#include "Drawer.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./solver inputFile.truss" << std::endl;
        exit(0);
    }

    std::cout << "Reading the input truss from file..." << std::endl;
    FileReader reader(argv[1]);
    Truss truss = reader.read();

    std::cout << "Drawing input truss into file \"input.svg\"..." << std::endl;
    Drawer drawer(truss, "input.svg");
    drawer.draw();

    std::cout << "Solving the truss..." << std::endl;
    Solver solver(truss);
    solver.solve();

    arma::Col<double> globalForces = solver.getGlobalForces();
    arma::Col<double> globalDisplacements = solver.getGlobalDisplacements();
    arma::Col<double> reactionForces = solver.getReactionForces();
    std::vector<arma::Col<double>> elementsInternalStress = solver.getElementsInternalStress();

    std::cout << "RESULTS" << std::endl;
    std::cout << "Global forces vector:" << std::endl;
    std::cout << globalForces << std::endl;
    std::cout << "Global displacements vector:" << std::endl;
    std::cout << globalDisplacements << std::endl;
    std::cout << "Reaction forces vector:" << std::endl;
    std::cout << reactionForces << std::endl;
    std::cout << "Elements internal stress vectors:" << std::endl;
    for (size_t i = 0; i < elementsInternalStress.size(); i++)
    {
        std::cout << "\tElement " << i << std::endl;
        std::cout << elementsInternalStress.at(i) << std::endl;
    }

    return 0;
}
