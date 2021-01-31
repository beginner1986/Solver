#include <iostream>
#include <vector>
#include <armadillo>

#include "FileReader.h"
#include "Truss.h"
#include "SolvedTruss.h"
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
    SolvedTruss solvedTruss(&truss);
    solvedTruss.solve();

    std::cout << "RESULTS" << std::endl;
    std::cout << "Global forces vector:" << std::endl;
    std::cout << solvedTruss.globalForces << std::endl;
    std::cout << "Global displacements vector:" << std::endl;
    std::cout << solvedTruss.globalDisplacements << std::endl;
    std::cout << "Reaction forces vector:" << std::endl;
    std::cout << solvedTruss.reactionForces << std::endl;
    std::cout << "Elements internal stress vectors:" << std::endl;
    for (size_t i = 0; i < solvedTruss.elementsInternalStress.size(); i++)
    {
        std::cout << "\tElement " << i << std::endl;
        std::cout << solvedTruss.elementsInternalStress.at(i) << std::endl;
    }

    return 0;
}
