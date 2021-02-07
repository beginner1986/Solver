#include <iostream>
#include <vector>
#include <armadillo>

#include "FileReader.h"
#include "Truss.h"
#include "SolvedTruss.h"
#include "FileWriter.h"

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

    std::cout << "Solving the truss..." << std::endl;
    SolvedTruss solvedTruss(truss);
    solvedTruss.solve();
    
    std::cout << "Drawing solved truss into file \"truss.svg\"..." << std::endl;
    solvedTruss.draw("truss.svg");

    std::cout << "RESULTS" << std::endl;
    std::cout << "Global forces vector:" << std::endl;
    std::cout << solvedTruss.globalForces << std::endl;
    std::cout << "Global displacements vector:" << std::endl;
    std::cout << solvedTruss.globalDisplacements << std::endl;
    std::cout << "Reaction forces vector:" << std::endl;
    for(double force : solvedTruss.reactionForces)
    {
        std::cout << force << std::endl;
    }
    std::cout << "Elements internal stress vectors:" << std::endl;
    for (size_t i = 0; i < solvedTruss.elementsInternalStress.size(); i++)
    {
        std::cout << "\tElement " << i << std::endl;
        std::cout << solvedTruss.elementsInternalStress.at(i) << std::endl;
    }

    FileWriter writer(solvedTruss);
    writer.save("result.csv");

    return 0;
}
