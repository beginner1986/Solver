#include "FileReader.h"

FileReader::FileReader(std::string fileName) : fileName(fileName)
{
    file.open(fileName,std::ios_base::in);



    file.close();
}

Truss& FileReader::read() 
{
    truss.A = readA();
    truss.E = readE();
    truss.coordinates = readCoordinates();
    truss.topology = readTopology();
    truss.constrains = readConstrains();
    truss.externalForces = readForces();
}

double FileReader::readA() 
{
    
}

double FileReader::readE() 
{
    
}

std::vector<double> FileReader::readCoordinates() 
{
    
}

arma::Mat<uint> FileReader::readTopology() 
{
    
}

std::vector<bool> FileReader::readConstrains() 
{
    
}

std::vector<double> FileReader::readForces() 
{
    
}
