#ifndef __FILEREADER_H__
#define __FILEREADER_H__

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <armadillo>

#include "Truss.h"

class FileReader
{
private:
    std::string fileName;
    std::ifstream file;

public:
    FileReader(std::string fileName);
    Truss read();

private:
    double readA();
    double readE();
    std::vector<double> readCoordinates();
    arma::Mat<uint> readTopology();
    std::vector<bool> readConstrains(size_t dofsCount);
    std::vector<double> readForces(size_t dofsCount);
    void fail();
};

#endif // __FILEREADER_H__