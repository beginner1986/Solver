#ifndef __FILEWRITER_H__
#define __FILEWRITER_H__

#include <string>
#include <fstream>
#include <armadillo>
#include <locale>

#include "SolvedTruss.h"

class FileWriter
{
private:
    arma::Col<double> globalForces;
    arma::Col<double> globalDisplacements;
    std::vector<double> reactionForces;
    std::vector<arma::Col<double>> elementsInternalStress;

public:
    FileWriter(const SolvedTruss &truss);
    void save(std::string fileName);
};

#endif // __FILEWRITER_H__