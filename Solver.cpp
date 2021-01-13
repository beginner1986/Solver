#include "Solver.h"

Solver::Solver(const Truss& truss) : truss(truss) 
{
    std::tie(sins, coss, lengths) = calculateSinCosLen();
    elementsStiffnessGlobal = calculateElementsStiffnessGlobal();  
    globalStiffnessMatrix = calculateGlobalStiffnessMatrix(elementsStiffnessGlobal);
    globalForces = truss.externalForces;
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double> > Solver::calculateSinCosLen() 
{
    std::vector<double> sins, coss, lengths;

    for(size_t element=0; element<truss.elementsCount; element++)
    {
        // element's degrees of freedoom matrix
        std::array<uint, 4> dofs = { 
            truss.topology(element, 0), 
            truss.topology(element, 1), 
            truss.topology(element, 2), 
            truss.topology(element, 3) 
        };

        // extract element's begin and end nodes coordinates
        double x1 = truss.coordinates.at(dofs[0]);
        double y1 = truss.coordinates.at(dofs[1]);
        double x2 = truss.coordinates.at(dofs[2]);
        double y2 = truss.coordinates.at(dofs[3]);

        // claculate the length and parameters for trigonometry
        double dx = abs(x2 - x1);
        double dy = abs(y2 - y1);
        double length = sqrt(dx * dx + dy * dy);
        lengths.push_back(length);

        // cosinus and sinus of the angle between local and global CS
        double cos = dx / length;
        coss.push_back(cos);
        double sin = dy / length;
        sins.push_back(sin);

        return std::make_tuple(sins, coss, lengths);
    }
}

std::vector<arma::Mat<double> > Solver::calculateElementsStiffnessGlobal()
{
    // element stiffness matrices in global CS
    std::vector<arma::Mat<double> > result;

    // template for element stiffness matrices
    arma::Mat<double> localStiffnessTemplate = {
        { 1, 0, -1, 0 },
        { 0, 0, 0, 0 },
        { -1, 0, 1, 0 },
        { 0, 0, 0, 0 }
    };

    for(size_t element=0; element<truss.elementsCount; element++)
    {
        // element stiffness value (scalar)
        double k = (truss.A * truss.E) / lengths.at(element);

        // element's stfiffness matrix in local CS
        arma::Mat<double> elementStiffnessLocal = localStiffnessTemplate;
        elementStiffnessLocal *= k;

        double sin = sins.at(element);
        double cos = coss.at(element);

        // transformation matrix
        arma::Mat<double> transtofrmationMatrix = {
            { cos * cos, cos * sin, -cos * cos, -cos * sin },
            { cos * sin, sin * sin, -cos * sin, -sin * sin },
            { -cos * cos, - cos * sin, cos * cos, cos * sin },
            { -cos * sin, -sin * sin, cos * sin, sin * sin }
        };

        // calculate element's stiffnesss matrix in global CS
        arma::Mat<double> elementStiffnessGlobal = transtofrmationMatrix * k;
        result.push_back(elementStiffnessGlobal);
    }

    return result;
}

arma::Mat<double> Solver::calculateGlobalStiffnessMatrix(const std::vector<arma::Mat<double> >& elementsStiffnessGlobal) 
{
    arma::Mat<double> result(truss.dofsCount, truss.dofsCount, arma::fill::zeros);

    for(size_t element=0; element<truss.elementsCount; element++)
    {
        // element's degrees of freedoom matrix
        std::array<uint, 4> dofs = { 
            truss.topology(element, 0), 
            truss.topology(element, 1), 
            truss.topology(element, 2), 
            truss.topology(element, 3) 
        };

        for(size_t i=0; i<4; i++)
            for(size_t j=0; j<4; j++)
                result(dofs[i], dofs[j]) += elementsStiffnessGlobal.at(element)(i, j);
    }
}

arma::Col<double> Solver::calculateGlobalDisplacements(const arma::Mat<double>& globalStiffnessMatrix, arma::Col<double>& globalForces)
{
    // global stiffness matrix and load vector reduction according to given constrains
    arma::Mat<double> reducedStiffness = globalStiffnessMatrix;
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

    // reduced displacements' vector
    arma::Col<double> reducedDisplacements(truss.dofsCount);
    arma::solve(reducedDisplacements, reducedStiffness, reducedForces);

    // global displacements' vector
    arma::Col<double> globalDispalcements(truss.dofsCount, arma::fill::zeros);
    
    uint count = 0;
    for(size_t dof=0; dof<truss.dofsCount; dof++)
    {
        if(!truss.constrains.at(dof))
            globalDispalcements(dof) = reducedDisplacements(count++);
    }

    return globalDispalcements;
}
