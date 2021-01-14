#include "Solver.h"

Solver::Solver(const Truss& truss) : truss(truss) 
{
    std::cout << "Elements' sin, cos and length values calculation in progress..." << std::endl;
    std::tie(sins, coss, lengths) = calculateSinCosLen();

    std::cout << "Elements' stiffness matrices in global CS calculation in progerss..." << std::endl;
    globalElementsStiffness = calculateElementsStiffnessGlobal();  

    std::cout << "Global stiffness matrix composition in progerss..." << std::endl;
    globalStiffnessMatrix = calculateGlobalStiffnessMatrix();

    std::cout << "Global forces vector initialization in progerss..." << std::endl;
    globalForces = truss.externalForces;

    std::cout << "Displacements in global CS calculation in progerss..." << std::endl;
    globalDisplacements = calculateGlobalDisplacements(globalStiffnessMatrix, globalForces);

    std::cout << "Elements' internal forces in global CS in progerss..." << std::endl;
    std::vector<arma::Col<double> > globalInternalForces = calculateGlobalInternalForces();

    std::cout << "Reaction forces calculation in progerss..." << std::endl;
    reactionForces = calculateReactionForces(globalInternalForces);

    std::cout << "Elements' internal stress calculation in progerss..." << std::endl;
    elementsInternalStress = calculateElementsInternalStress(globalInternalForces);
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
    }

    return std::make_tuple(sins, coss, lengths);
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

        // transformation matrix
        double sin = sins.at(element);
        double cos = coss.at(element);

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

arma::Mat<double> Solver::calculateGlobalStiffnessMatrix() 
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
                result(dofs[i], dofs[j]) += globalElementsStiffness.at(element)(i, j);
    }

    return result;
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

std::vector<arma::Col<double> > Solver::calculateGlobalInternalForces() 
{
    std::vector<arma::Col<double> > result;

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
            elementDisplacements(j) = globalDisplacements(dofs[j]);
        
        // elements' internal forces (stress)
        arma::Col<double> globalElementInternalForces(4);
        globalElementInternalForces = globalElementsStiffness.at(element) * elementDisplacements;
        result.push_back(globalElementInternalForces);
    }

    return result;
}

arma::Col<double> Solver::calculateReactionForces(std::vector<arma::Col<double> >& globalInternalForces) 
{
    arma::Col<double> result(truss.dofsCount, arma::fill::zeros);
    
    for(size_t element=0; element<truss.elementsCount; element++)
    {
        // degrees of freedom
        std::array<uint, 4> dofs = { 
            truss.topology(element, 0), 
            truss.topology(element, 1), 
            truss.topology(element, 2), 
            truss.topology(element, 3) 
        };

        // if current dof is fixed, then include it's value into the reactions vector
        for(size_t i=0; i<4; i++)
        {
            if(truss.constrains.at(dofs[i]))
                result(dofs[i]) += globalInternalForces.at(element)(i);
        }
    }

    return result;
}

std::vector<arma::Col<double> > Solver::calculateElementsInternalStress(std::vector<arma::Col<double> >& globalInternalForces) 
{
    std::vector<arma::Col<double> > result;

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
        
        solve(internalForcesLocal, transformationMatrix, globalInternalForces.at(element));
        result.push_back(internalForcesLocal);
    }

    return result;
}