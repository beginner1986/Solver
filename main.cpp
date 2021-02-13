#include <iostream>
#include <vector>
#include <armadillo>
#include <regex>

#include "FileReader.h"
#include "Truss.h"
#include "SolvedTruss.h"
#include "FileWriter.h"

#define OUTPUT_FOLDER "output/"

std::string makeFileName(std::string path);
void printResults(const SolvedTruss &solvedTruss);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./solver inputFile.truss" << std::endl;
        std::cout << "\t-noout\t- don't print results on the screen, no SVG or CVS file generated." << std::endl;
        std::cout << "\t-sparse\t- saprse global stiffness matrix." << std::endl;
        std::cout << "\t-dense\t- dense global stiffness matrix." << std::endl;

        exit(0);
    }

    std::vector<std::string> parameters;
    for(int i=0; i<argc; i++)
        parameters.push_back(std::string(argv[i]));

    std::string path = argv[1];
    std::cout << "Reading the input truss from file..." << std::endl;
    FileReader reader(path);
    Truss truss = reader.read();

    std::cout << "Solving the truss..." << std::endl;
    SolvedTruss solvedTruss(truss);
    SOLVER_OPTS solverOptions;

    if(std::find(parameters.begin(), parameters.end(), "-dense") != parameters.end())
    {
        std::cout << "DENSE solver" << std::endl;
        solverOptions = SOLVER_OPTS::DENSE;
    }
    else if(std::find(parameters.begin(), parameters.end(), "-sparse") != parameters.end())
    {
        std::cout << "SPARSE solver" << std::endl;
        solverOptions = SOLVER_OPTS::SPARSE;
    }
    else
    {
        std::cout << "ERROR: you must specify solver type as program parameter: -dense or -sparse" << std::endl;
        exit(-1);
    }

    solvedTruss.solve(solverOptions);
    
    if(std::find(parameters.begin(), parameters.end(), "-noout") == parameters.end())
    {
        printResults(solvedTruss);

        std::string fileName = makeFileName(path);
        std::cout << "Drawing solved truss into SVG file..." << std::endl;
        solvedTruss.draw(fileName + ".svg");

        std::cout << "Sawing calculations results into CSV file..." << std::endl;
        FileWriter writer(solvedTruss);
        writer.save(fileName + ".csv");
    }

    return 0;
}

std::string makeFileName(std::string path)
{
    std::string fileName = path.substr(0, path.size() - 6);
    fileName = std::regex_replace(fileName, std::regex("(.*\\/)|(\\..*)"), "");
    fileName = OUTPUT_FOLDER + fileName;

    return fileName;
}

void printResults(const SolvedTruss &solvedTruss)
{
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
}
