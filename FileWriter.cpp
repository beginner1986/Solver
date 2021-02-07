#include "FileWriter.h"


FileWriter::FileWriter(const SolvedTruss &truss) 
    : globalForces(truss.globalForces), 
        globalDisplacements(truss.globalDisplacements), 
        reactionForces(truss.reactionForces),
        elementsInternalStress(truss.elementsInternalStress)
{
    
}

void FileWriter::save(std::string fileName) 
{
    std::ofstream file;
    file.open(fileName,std::ios::out);
    
    if(!file.is_open())
    {
        std::cout << "Cannot create file " << fileName << "." << std::endl;
        exit(-1);
    }

    file << "Global forces vector:" << std::endl;
    file << globalForces << std::endl;
    /*
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
    */
}
