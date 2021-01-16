#include "FileReader.h"

FileReader::FileReader(std::string fileName) : fileName(fileName)
{
    file.open(fileName,std::ios::in);
    std::string input;
    std::getline(file, input);
    std::cout << input << std::endl;
    if(input != "TRUSS")
    {
        std::cout << "Incorrect input file!" << std::endl;
        file.close();
    }
}

Truss& FileReader::read() 
{
    if(!file.is_open())
    {
        std::cout << "Cannot continue." << std::endl;
        exit(-1);
    }
    
    truss.A = readA();
    truss.E = readE();
    truss.coordinates = readCoordinates();
    truss.topology = readTopology();
    truss.constrains = readConstrains();
    truss.externalForces = readForces();

    file.close();
}

double FileReader::readA() 
{
    
}

double FileReader::readE() 
{
    
}

std::vector<double>& FileReader::readCoordinates() 
{
    
}

arma::Mat<uint>& FileReader::readTopology() 
{
    
}

std::vector<bool>& FileReader::readConstrains() 
{
    
}

std::vector<double>& FileReader::readForces() 
{
    
}
