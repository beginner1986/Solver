#include "FileReader.h"

FileReader::FileReader(std::string fileName) : fileName(fileName)
{
    file.open(fileName,std::ios::in);
    std::string line;
    std::getline(file, line);
    if(line != "TRUSS")
    {
        std::cout << "Incorrect input file!" << std::endl;
        file.close();
    }
}

Truss& FileReader::read() 
{
    if(!file.is_open())
    {
        fail();
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
    std::string line, word;
    std::getline(file, line);
    std::istringstream iss(line);

    iss >> word;
    if(word != "A")
    {
        std::cout << "Incorrect A line!" << std::endl;
        fail();
    }

    iss >> word;
    return std::stod(word);
}

double FileReader::readE() 
{
    std::string line, word;
    std::getline(file, line);
    std::istringstream iss(line);

    iss >> word;
    if(word != "E")
    {
        std::cout << "Incorrect E line!" << std::endl;
        fail();
    }

    iss >> word;
    return std::stod(word);
}

std::vector<double> FileReader::readCoordinates() 
{
    std::vector<double> coordinates;

    std::string line, word;
    std::getline(file, line);
    std::istringstream iss(line);

    iss >> word;
    if(word != "DOFS")
    {
        std::cout << "Incorrect DOFS line!" << std::endl;
        fail();
    }

    iss >> word;
    size_t dofsCount = static_cast<size_t>(std::stoi(word));
    for(size_t dof = 0; dof < dofsCount; dof++)
    {
        iss >> word;
        double coord = std::stod(word);
        coordinates.push_back(coord);
    }

    return coordinates;
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

void FileReader::fail() 
{
    std::cout << "Cannot continue." << std::endl;

    if(file.is_open())
    {
        file.close();
    }

    exit(-1);
}
