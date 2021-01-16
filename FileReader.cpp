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
    size_t dofsCount = truss.constrains.size();
    truss.constrains = readConstrains(dofsCount);
    truss.externalForces = readForces(dofsCount);

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
    for(size_t node = 0; node < dofsCount/2; node++)
    {
        std::getline(file, line);
        iss = std::istringstream(line);
        for (size_t i = 0; i < 2; i++)
        {
            iss >> word;
            double coord = std::stod(word);
            coordinates.push_back(coord);
        }
    }

    return coordinates;
}

arma::Mat<uint> FileReader::readTopology() 
{
    std::string line, word;
    std::getline(file, line);
    std::istringstream iss(line);

    iss >> word;
    if(word != "BARS")
    {
        std::cout << "Incorrect BARS line!" << std::endl;
        fail();
    }
    iss >> word;
    size_t elementsCount = static_cast<size_t>(std::stoi(word));
    arma::Mat<uint> topology(elementsCount, 4);

    for(size_t element=0; element < elementsCount; element++)
    {
        std::getline(file, line);
        iss = std::istringstream(line);

        for(int dof=0; dof<4; dof++)
        {
            iss >> word;
            topology(element, dof) = static_cast<uint>(std::stoi(word));
        }
    }

    return topology;
}

std::vector<bool> FileReader::readConstrains(size_t dofsCount) 
{
    std::vector<bool> constrains;
    std::string line, word;
    std::getline(file, line);
    std::istringstream iss(line);

    iss >> word;
    if(word != "CONSTRAINS")
    {
        std::cout << "Incorrect CONSTRAINS line!" << std::endl;
        fail();
    }

    for (size_t node = 0; node < dofsCount/2; node++)
    {
        std::getline(file, line);
        iss = std::istringstream(line);
        
        for (size_t i = 0; i < 2; i++)
        {
            iss >> word;
            bool is_fixed = static_cast<bool>(std::stoi(word));
            constrains.push_back(is_fixed);
        }
    }

    return constrains;
}

std::vector<double> FileReader::readForces(size_t dofsCount) 
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
