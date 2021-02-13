#include <iostream>
#include <vector>
#include <armadillo>
#include <regex>
#include <chrono>

#include "FileReader.h"
#include "Truss.h"
#include "SolvedTruss.h"
#include "FileWriter.h"

#define OUTPUT_FOLDER "output/"

void printUsage();
std::string makeFileName(std::string path);
void printResults(const SolvedTruss &solvedTruss);

int main(int argc, char *argv[])
{
    auto start = std::chrono::steady_clock::now();

    if (argc < 2)
        printUsage();

    std::vector<std::string> parameters;
    for(int i=0; i<argc; i++)
        parameters.push_back(std::string(argv[i]));

    bool times = false;
    if(std::find(parameters.begin(), parameters.end(), "-times") != parameters.end())
        times = true;

    // read data from file
    std::string path = argv[1];
    std::cout << path << std::endl;
    FileReader reader(path);
    Truss truss = reader.read();

    // solving the truss
    SolvedTruss solvedTruss(truss);
    SOLVER_OPTS solverOptions;

    // choose the solver
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

    auto solveStart = std::chrono::steady_clock::now();
    solvedTruss.solve(solverOptions);
    auto solveEnd = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = solveEnd - solveStart;
    if(times)
        std::cout << elapsed.count() << "s" << std::endl;
    
    // generate outputs *.svg & *.csv
    if(std::find(parameters.begin(), parameters.end(), "-noout") == parameters.end())
    {
        printResults(solvedTruss);

        std::string fileName = makeFileName(path);
        solvedTruss.draw(fileName + ".svg");

        FileWriter writer(solvedTruss);
        writer.save(fileName + ".csv");
    }

    auto end = std::chrono::steady_clock::now();
    elapsed = end - start;
    if(times)
        std::cout << elapsed.count() << "s" << std::endl;

    return 0;
}

void printUsage() 
{
    std::cout << "Usage: ./solver inputFile.truss" << std::endl;
    std::cout << "\t-noout\t- don't print results on the screen, no SVG or CVS file generated." << std::endl;
    std::cout << "\t-sparse\t- saprse global stiffness matrix." << std::endl;
    std::cout << "\t-dense\t- dense global stiffness matrix." << std::endl;
    std::cout << "\t-times\t- print solving time and program summary time." << std::endl;

    exit(0);
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
