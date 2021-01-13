#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <armadillo>

#include "Truss.h"
#include "Solver.h"

int main()
{
    // analyzed truss object
    Truss truss(1, 1);

    Solver& solver = Solver::getInstance();

    // global stiffness matrix declaration
    arma::Mat<double> globalStiffness(truss.dofsCount, truss.dofsCount);

    // template for elements' stiffness matrices
    arma::Mat<double> localStiffnessTemplate(4, 4, arma::fill::zeros);
    localStiffnessTemplate(0, 0) = 1;
    localStiffnessTemplate(0, 2) = -1;
    localStiffnessTemplate(2, 0) = -1;
    localStiffnessTemplate(2, 2) = 1;

    // sinus, cosinus and lenghts ot each element is stored for further stress computation
    std::vector<double> sins, coss, lengths;

    // elements' stiffness matrices in global CS stored for internal forces computation
    std::vector<arma::Mat<double> > elementsStiffnessGlobalMatrices;

    for(size_t element=0; element<truss.elementsCount; element++)
    {
        std::cout << "ELEMENT " << element << std::endl;
        // element's degrees of freenoom matrix
        std::array<uint, 4> dofs = { 
            truss.topology(element, 0), 
            truss.topology(element, 1), 
            truss.topology(element, 2), 
            truss.topology(element, 3) 
        };

        // extract element's begin and end coordinates
        double x1 = truss.coordinates.at(dofs[0]);
        double y1 = truss.coordinates.at(dofs[1]);
        double x2 = truss.coordinates.at(dofs[2]);
        double y2 = truss.coordinates.at(dofs[3]);
        std::cout << "\t(" << x1 << ", " << y1 << ") - (" << x2 << ", " << y2 << ")" << std::endl;

        // claculate the length and parameters for trigonometry
        double dx = abs(x2 - x1);
        double dy = abs(y2 - y1);
        double length = sqrt(dx * dx + dy * dy);
        lengths.push_back(length);
        std::cout << "\tdx = " << dx << "; dy = " << dy << "; l = " << length << std::endl;

        // cosinus and sinus of the angle between local and global CS
        double cos = dx / length;
        coss.push_back(cos);
        double sin = dy / length;
        sins.push_back(sin);
        std::cout << "\tcos = " << cos << "; sin =" << sin << std::endl;

        // beam stiffness value (scalar)
        double k = (truss.A * truss.E) / length;
        std::cout << "\tk = " << k << std::endl;

        // element's stfiffness matrix in local CS
        arma::Mat<double> elementStiffnessLocal = localStiffnessTemplate;
        elementStiffnessLocal *= k;

        std::cout << "\t element stiffness in local CS:" << std::endl;
        std::cout << elementStiffnessLocal << std::endl;

        // transformation matrix
        arma::Mat<double> transtofrmationMatrix = {
            { cos * cos, cos * sin, -cos * cos, -cos * sin },
            { cos * sin, sin * sin, -cos * sin, -sin * sin },
            { -cos * cos, - cos * sin, cos * cos, cos * sin },
            { -cos * sin, -sin * sin, cos * sin, sin * sin }
        };
        std::cout << "\tTransformation matrix:" << std::endl;
        std::cout << transtofrmationMatrix << std::endl;

        // calculate element's stiffnesss matrix in global CS
        arma::Mat<double> elementStiffnessGlobal = transtofrmationMatrix * k;
        elementsStiffnessGlobalMatrices.push_back(elementStiffnessGlobal);

        std::cout << "\tElement stiffness matrix in global CS:" << std::endl;
        std::cout << elementStiffnessGlobal << std::endl;

        // assembly global stiffness matrix [4x4]
        for(size_t i=0; i<4; i++)
            for(size_t j=0; j<4; j++)
                globalStiffness(dofs[i], dofs[j]) += elementStiffnessGlobal(i, j);
    }

    std::cout << "Global stiffness matrix: " << std::endl;
    std::cout << globalStiffness << std::endl;

    // global forces vector
    arma::Col<double> globalForces(truss.externalForces);
    std::cout << "Global forces vector: " << std::endl;
    std::cout << globalForces << std::endl;

    // global stiffness matrix and load vector reduction according to given constrains
    arma::Mat<double> reducedStiffness = globalStiffness;
    arma::Col<double> reducedForces = globalForces;

    for(int dof=truss.dofsCount-1; dof>=0; dof--)
    {
        if(truss.constrains.at(dof))
        {
            reducedStiffness.shed_col(dof);
            reducedStiffness.shed_row(dof);
            reducedForces.shed_row(dof);
        }
    }

    std::cout << "Reduced stiffness matrix:" << std::endl;
    std::cout << reducedStiffness << std::endl;

    std::cout << "Reduced loads' vector:" << std::endl;
    std::cout << reducedForces << std::endl; 

    // reduced displacements' vector
    arma::Col<double> reducedDisplacements(truss.dofsCount);
    arma::solve(reducedDisplacements, reducedStiffness, reducedForces);
    std::cout << "Reduced displacements' vector: " << std::endl;
    std::cout << reducedDisplacements << std::endl;

    // global displacements' vector
    arma::Col<double> dispalcementsGlobal(truss.dofsCount, arma::fill::zeros);
    
    uint count = 0;
    for(size_t dof=0; dof<truss.dofsCount; dof++)
    {
        if(!truss.constrains.at(dof))
            dispalcementsGlobal(dof) = reducedDisplacements(count++);
    }

    std::cout << "Global displacements vector: " << std::endl;
    std::cout << dispalcementsGlobal << std::endl;

    // reaction forces caused by the constrains
    arma::Col<double> reactions(truss.dofsCount, arma::fill::zeros);
    std::vector<arma::Col<double> > internalForcesGlobalVectors;

    // elements internal forces
    for(size_t element=0; element<truss.elementsCount; element++)
    {
        // degrees of freedom
        std::array<uint, 4> dofs = { 
            truss.topology(element, 0), 
            truss.topology(element, 1), 
            truss.topology(element, 2), 
            truss.topology(element, 3) 
        };

        // element's displacements in global CS
        arma::Col<double> elementDisplacements(4);
        for(size_t j=0; j<4; j++)
        {
            elementDisplacements(j) = dispalcementsGlobal(dofs[j]);
        }

        // elements' internal forces (stress)
        arma::Col<double> internalForcesGlobal(4);
        internalForcesGlobal = elementsStiffnessGlobalMatrices.at(element) * elementDisplacements;
        internalForcesGlobalVectors.push_back(internalForcesGlobal);

        // if current dof is fixed, then include it's value into the reactions vector
        for(size_t j=0; j<4; j++)
        {
            if(truss.constrains.at(dofs[j]))
                reactions(dofs[j]) += internalForcesGlobal(j);
        }

        std::cout << "ELEMENT " << element << " internal forces vector: " << std::endl;
        std::cout << internalForcesGlobal << std::endl;
    }

    std::cout << "Reaction forces: " << std::endl;
    std::cout << reactions << std::endl;

    // internal forces in local CS
    std::vector<arma::Col<double> > internalForcesLocalVectors;

    std::cout << "Internal forces in local coordinate systems:" << std::endl;
    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        arma::Col<double> internalForcesLocal(4);

        arma::Mat<double> transformationMatrix = {
            { coss[element], sins[element], 0, 0 },
            { -sins[element], coss[element], 0, 0 },
            { 0, 0, coss[element], sins[element] },
            { 0, 0, -sins[element], coss[element] }
        };
        
        solve(internalForcesLocal, transformationMatrix, internalForcesGlobalVectors.at(element));
        internalForcesLocalVectors.push_back(internalForcesLocal);

        std::cout << "\tELEMENT " << element << " :" << std::endl;
        std::cout << internalForcesLocal << std::endl;
    }

    return 0;
}
