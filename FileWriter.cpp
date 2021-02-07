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

    size_t dofsCount = globalDisplacements.size();

    file << "DOF #; ";
    for(size_t i=0; i<dofsCount; i++)
        file << i << "; ";
    file << std::endl;

    file << "Global forces; ";
    for(double f : globalForces)
        file << f << "; ";
    file << std::endl;

    file << "Global displacements; ";
    for(double d : globalDisplacements)
        file << d << "; ";
    file << std::endl;

    file << "Reaction forces; ";
    for(double r : reactionForces)
        file << r << "; ";
    file << std::endl << std::endl;

    file << "Element #; x1; y1; x2; y2" << std::endl;
    for(size_t e=0; e<elementsInternalStress.size(); e++)
    {
        file << e << "; ";
        for(double s : elementsInternalStress.at(e))
            file << s << "; ";
        
        file << std::endl;
    }
}
