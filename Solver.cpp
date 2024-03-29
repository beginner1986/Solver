#include <cmath>
#include <limits>
#include <chrono>
#include <fstream>
#include "Solver.h"

void luSolve(arma::Col<double> &displacements, arma::Mat<double> &tiffness, arma::Col<double> &forces);
void qrSolve(arma::Col<double> &displacements, arma::Mat<double> &stiffness, arma::Col<double> &forces);
void jacobiSolve(arma::Col<double> &displacements, arma::Mat<double> &stiffness, arma::Col<double> &forces);
void gaussSeidelSolve(arma::Col<double> &displacements, arma::Mat<double> &stiffness, arma::Col<double> &forces);
void drawMatrix(arma::Mat<double> mat);

void Solver::solve(SOLVER_OPTS opts, bool times)
{
    auto totalTimeStart = std::chrono::steady_clock::now();
    std::tie(sins, coss, lengths) = calculateSinCosLen();
    globalElementsStiffness = calculateElementsStiffnessGlobal();
    globalForces = truss.externalForces;

    arma::Mat<double> globalStiffnessMatrix;
    arma::SpMat<double> globalStiffnessSparseMatrix;

    auto solveTimeStart = std::chrono::steady_clock::now();
    auto solveTimeEnd = std::chrono::steady_clock::now();
    if(opts == SOLVER_OPTS::DENSE)
    {
        globalStiffnessMatrix = calculateGlobalStiffnessMatrix();
        solveTimeStart = std::chrono::steady_clock::now();
        globalDisplacements = calculateGlobalDisplacements(globalStiffnessMatrix, globalForces);
        solveTimeEnd = std::chrono::steady_clock::now();
    }
    else if(opts == SOLVER_OPTS::SPARSE)
    {
        globalStiffnessSparseMatrix = calculateGlobalStiffnessSparseMatrix();
        solveTimeStart = std::chrono::steady_clock::now();
        globalDisplacements = calculateGlobalDisplacements(globalStiffnessSparseMatrix, globalForces);
        solveTimeEnd = std::chrono::steady_clock::now();
    }

    // equations system solving time
    if(times)
    {
        std::chrono::duration<double> solveElapsed = solveTimeEnd - solveTimeStart;
        std::cout << solveElapsed.count() << "; ";
    }

    std::vector<arma::Col<double>> globalInternalForces = calculateGlobalInternalForces();
    reactionForces = calculateReactionForces(globalInternalForces);
    elementsInternalStress = calculateElementsInternalStress(globalInternalForces);

    // total MES solving time
    auto totalTimeEnd = std::chrono::steady_clock::now();
    if(times)
    {
        std::chrono::duration<double> totalElapsed = totalTimeEnd - totalTimeStart;
        std::cout << totalElapsed.count() << "; ";
    }
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> Solver::calculateSinCosLen()
{
    std::vector<double> sins, coss, lengths;

    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        // element's degrees of freedoom matrix
        std::array<uint, 4> dofs = {
            truss.topology(element, 0),
            truss.topology(element, 1),
            truss.topology(element, 2),
            truss.topology(element, 3)};

        // extract element's begin and end nodes coordinates
        double x1 = truss.coordinates.at(dofs[0]);
        double y1 = truss.coordinates.at(dofs[1]);
        double x2 = truss.coordinates.at(dofs[2]);
        double y2 = truss.coordinates.at(dofs[3]);

        double dx = abs(x2 - x1);
        double dy = abs(y2 - y1);
        double length = sqrt(dx * dx + dy * dy);
        lengths.push_back(length);

        double cos = dx / length;
        coss.push_back(cos);
        double sin = dy / length;
        sins.push_back(sin);
    }

    return std::make_tuple(sins, coss, lengths);
}

std::vector<arma::Mat<double>> Solver::calculateElementsStiffnessGlobal()
{
    std::vector<arma::Mat<double>> result;

    // template for element stiffness matrices
    arma::Mat<double> localStiffnessTemplate = {
        {1, 0, -1, 0},
        {0, 0, 0, 0},
        {-1, 0, 1, 0},
        {0, 0, 0, 0}
    };

    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        // element stiffness value (scalar)
        double k = (truss.A * truss.E) / lengths.at(element);

        arma::Mat<double> localElementStiffness = localStiffnessTemplate;
        localElementStiffness *= k;

        double sin = sins.at(element);
        double cos = coss.at(element);

        arma::Mat<double> transtofrmationMatrix = {
            {cos * cos, cos * sin, -cos * cos, -cos * sin},
            {cos * sin, sin * sin, -cos * sin, -sin * sin},
            {-cos * cos, -cos * sin, cos * cos, cos * sin},
            {-cos * sin, -sin * sin, cos * sin, sin * sin}
        };

        arma::Mat<double> elementStiffnessGlobal = transtofrmationMatrix * k;
        result.push_back(elementStiffnessGlobal);
    }

    return result;
}

arma::Mat<double> Solver::calculateGlobalStiffnessMatrix()
{
    arma::Mat<double> result(truss.dofsCount, truss.dofsCount, arma::fill::zeros);

    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        // element's degrees of freedoom matrix
        std::array<uint, 4> dofs = {
            truss.topology(element, 0),
            truss.topology(element, 1),
            truss.topology(element, 2),
            truss.topology(element, 3)
        };

        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                arma::Mat<double> &elementStiffes = globalElementsStiffness.at(element);
                result(dofs[i], dofs[j]) += elementStiffes(i, j);
            }
        }
    }

    // drawMatrix(result);

    return result;
}

arma::SpMat<double> Solver::calculateGlobalStiffnessSparseMatrix() 
{
    arma::SpMat<double> result(truss.dofsCount, truss.dofsCount);

    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        // element's degrees of freedoom matrix
        std::array<uint, 4> dofs = {
            truss.topology(element, 0),
            truss.topology(element, 1),
            truss.topology(element, 2),
            truss.topology(element, 3)
        };

        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                arma::Mat<double> &elementStiffes = globalElementsStiffness.at(element);
                result(dofs[i], dofs[j]) += elementStiffes(i, j);
            }
        }
    }

    return result;
}

arma::Col<double> Solver::calculateGlobalDisplacements(const arma::Mat<double> &globalStiffnessMatrix, arma::Col<double> &globalForces)
{
    arma::Mat<double> reducedStiffness = globalStiffnessMatrix;
    arma::Col<double> reducedForces = globalForces;

    for (int dof = truss.dofsCount - 1; dof >= 0; dof--)
    {
        if (truss.constrains.at(dof))
        {
            reducedStiffness.shed_col(dof);
            reducedStiffness.shed_row(dof);
            reducedForces.shed_row(dof);
        }
    }

    arma::Col<double> reducedDisplacements(reducedForces.n_rows);
    luSolve(reducedDisplacements, reducedStiffness, reducedForces);

    arma::Col<double> r = reducedForces - reducedStiffness * reducedDisplacements;
    std::cout << " " << std::max(std::abs(r.max()), std::abs(r.min())) << "; ";

    arma::Col<double> globalDispalcements(truss.dofsCount, arma::fill::zeros);

    uint count = 0;
    for (size_t dof = 0; dof < truss.dofsCount; dof++)
    {
        if (!truss.constrains.at(dof))
        {
            globalDispalcements(dof) = reducedDisplacements(count);
            count++;
        }
    }

    return globalDispalcements;
}

arma::Col<double> Solver::calculateGlobalDisplacements(const arma::SpMat<double> &globalStiffnessMatrix, arma::Col<double> &globalForces) 
{
    arma::SpMat<double> reducedStiffness = globalStiffnessMatrix;
    arma::Col<double> reducedForces = globalForces;

    for (int dof = truss.dofsCount - 1; dof >= 0; dof--)
    {
        if (truss.constrains.at(dof))
        {
            reducedStiffness.shed_col(dof);
            reducedStiffness.shed_row(dof);
            reducedForces.shed_row(dof);
        }
    }

    arma::Col<double> reducedDisplacements(reducedForces.n_rows);
    reducedDisplacements = arma::spsolve(reducedStiffness, reducedForces);

    arma::Col<double> r = reducedForces - reducedStiffness * reducedDisplacements;
    std::cout << " " << std::max(std::abs(r.max()), std::abs(r.min())) << ";";

    arma::Col<double> globalDispalcements(truss.dofsCount, arma::fill::zeros);

    uint count = 0;
    for (size_t dof = 0; dof < truss.dofsCount; dof++)
    {
        if (!truss.constrains.at(dof))
        {
            globalDispalcements(dof) = reducedDisplacements(count);
            count++;
        }
    }

    return globalDispalcements;
}

std::vector<arma::Col<double>> Solver::calculateGlobalInternalForces()
{
    std::vector<arma::Col<double>> result;

    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        // degrees of freedom
        std::array<uint, 4> dofs = {
            truss.topology(element, 0),
            truss.topology(element, 1),
            truss.topology(element, 2),
            truss.topology(element, 3)
        };

        arma::Col<double> globalElementDisplacements(4);
        for (size_t j = 0; j < 4; j++)
        {
            globalElementDisplacements(j) = globalDisplacements(dofs[j]);
        }

        arma::Col<double> globalElementInternalForces(4);
        globalElementInternalForces = globalElementsStiffness.at(element) * globalElementDisplacements;
        result.push_back(globalElementInternalForces);
    }

    return result;
}

std::vector<double> Solver::calculateReactionForces(std::vector<arma::Col<double>> &globalInternalForces)
{
    std::vector<double> result(truss.dofsCount, 0);

    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        // degrees of freedom
        std::array<uint, 4> dofs = {
            truss.topology(element, 0),
            truss.topology(element, 1),
            truss.topology(element, 2),
            truss.topology(element, 3)};

        for (size_t i = 0; i < 4; i++)
        {
            if (truss.constrains.at(dofs[i]))
            {
                arma::Col<double> &globalElementsInternalForces = globalInternalForces.at(element);
                result.at(dofs[i]) += globalElementsInternalForces(i);
            }
        }
    }

    return result;
}

std::vector<arma::Col<double>> Solver::calculateElementsInternalStress(std::vector<arma::Col<double>> &globalInternalForces)
{
    std::vector<arma::Col<double>> result;

    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        arma::Col<double> internalForcesLocal(4);

        arma::Mat<double> transformationMatrix = {
            {coss[element], sins[element], 0, 0},
            {-sins[element], coss[element], 0, 0},
            {0, 0, coss[element], sins[element]},
            {0, 0, -sins[element], coss[element]}
        };

        arma::solve(internalForcesLocal, transformationMatrix.i(), globalInternalForces.at(element));
        result.push_back(internalForcesLocal);
    }

    return result;
}

void luSolve(arma::Col<double> &displacements, arma::Mat<double> &stiffness, arma::Col<double> &forces)
{
    arma::Mat<double> L, U;
    arma::lu(L, U, stiffness);
    displacements = U.i() * L.i() * forces;
}

void qrSolve(arma::Col<double> &displacements, arma::Mat<double> &stiffness, arma::Col<double> &forces)
{
    arma::Mat<double> Q, R;
    arma::qr(Q, R, stiffness);
    displacements = R.i() * Q.t() * forces;
}

void jacobiSolve(arma::Col<double> &displacements, arma::Mat<double> &stiffness, arma::Col<double> &forces)
{
    size_t n = displacements.n_rows;
    double normVal = std::numeric_limits<double>::max();
    double tolerance = 1e-4; 
    uint iterations = 0;

    while(normVal > tolerance)
    {
        arma::Col<double> oldDisplacements = displacements;
        
        for(size_t i=0; i<n; i++)
        {
            double sigma = 0;
            
            for(size_t j=0; j<n; j++)
            {
                if(i != j)
                    sigma = sigma + stiffness(i,j) * displacements(j);
            }
            
            displacements(i) = (1 / stiffness(i,i)) * (forces(i) - sigma);
        }
        
        iterations++;

        double maxDiff = 0;
        for(size_t i=0; i<n; i++)
        {
            double diff = oldDisplacements(i) - displacements(i);
            if(diff > maxDiff)
                maxDiff = diff;
        }
        normVal = maxDiff;
    }
}

void gaussSeidelSolve(arma::Col<double> &displacements, arma::Mat<double> &stiffness, arma::Col<double> &forces)
{
    size_t n = displacements.n_rows;
    double normVal = std::numeric_limits<double>::max();
    double tolerance = 1e-4; 
    uint iterations = 0;
    
    while(normVal > tolerance)
    {
        arma::Col<double> oldDisplacements = displacements;

        for(size_t i=0; i<n; i++)
        {
            double sigma = 0;
            
            for(size_t j=0; j<n; j++)
            {
                if(i != j)
                    sigma += (stiffness(i,j) * displacements(j));
            }
            
            displacements(i) = (1 / stiffness(i,i)) * (forces(i) - sigma);
        }
        
        iterations++;

        double maxDiff = 0;
        for(size_t i=0; i<n; i++)
        {
            double diff = oldDisplacements(i) - displacements(i);
            if(diff > maxDiff)
                maxDiff = diff;
        }
        normVal = maxDiff;
    }
}

void drawMatrix(arma::Mat<double> mat)
{
    std::string fileName = "matrix.pbm";
    std::ofstream file;
    file.open(fileName, std::ios::out);
    
    if(!file.is_open())
    {
        std::cout << "Cannot create file " << fileName << "." << std::endl;
        exit(-1);
    }

    file << "P1" << std::endl << mat.n_cols << " " << mat.n_rows << std::endl;
    
    for(size_t i=0; i<mat.n_cols; i++)
    {
        for(size_t j=0; j<mat.n_rows; j++)
        {
            file << ((mat(i, j) != 0) ? 1 : 0) << " ";
        }
        file << std::endl;
    }
}