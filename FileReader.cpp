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

Truss FileReader::read() 
{
    if(!file.is_open())
    {
        fail();
    }
    
    double A = readA();
    double E = readE();
    std::vector<double> coordinates = readCoordinates();
    arma::Mat<uint> topology = readTopology();
    size_t dofsCount = coordinates.size();
    std::vector<bool> constrains = readConstrains(dofsCount);
    std::vector<double> externalForces = readForces(dofsCount);

    file.close();

    return Truss(A, E, coordinates, topology, constrains, externalForces);
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
        std::cout << "Incorrect data before BARS line!" << std::endl;
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
        std::cout << "Incorrect data before CONSTRAINS line!" << std::endl;
        fail();
    }

    for(size_t node = 0; node < dofsCount/2; node++)
    {
        std::getline(file, line);
        iss = std::istringstream(line);
        for (size_t i = 0; i < 2; i++)
        {
            iss >> word;
            bool isFixed = static_cast<bool>(std::stoi(word));
            constrains.push_back(isFixed);
        }
    }

    return constrains;
}

std::vector<double> FileReader::readForces(size_t dofsCount) 
{
    std::vector<double> forces;
    std::string line, word;
    std::getline(file, line);
    std::istringstream iss(line);
    
    iss >> word;
    if(word != "FORCES")
    {
        std::cout << "Incorrect data before FORCES line!" << std::endl;
        fail();
    }

    for (size_t node = 0; node < dofsCount/2; node++)
    {
        std::getline(file, line);
        iss = std::istringstream(line);
        
        for (size_t i = 0; i < 2; i++)
        {
            iss >> word;
            double force = std::stod(word);
            forces.push_back(force);
        }
    }

    return forces;
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
