#include <iostream>
#include <vector>
#include <cmath>
#include <armadillo>

int main()
{
    //matherial properties
    double A = 1;       // elements' cross area
    double E = 1;       // Young's modulus

    // nodes coordinates definition (each node x and y) in one table as they are also dofs
    const size_t nodesCount = 3;
    const size_t dofsCount = 2 * nodesCount;
    double coordinates[dofsCount] = { 
        0, 0,       // node 0
        1, 0,       // node 1
        1, 1,       // node 2
    };

    // nodes connections by the elements
    const size_t elementsCount = 3;
    uint topology[elementsCount][4] = {
        { 0, 1, 2, 3 },     // node 1 to 1
        { 2, 3, 4, 5 },     // node 1 to 2
        { 0, 1, 4, 5 },     // node 0 to 2
    };

    // constrains of each node and x and y directions (true=fixed, false=not fixed)
    bool constrains[dofsCount] = { 
        true, true,     // node 0 fixed along x and y axis
        true, true,     // node 1 fixed along x and y axis
        false, false,   // node 2 not fixed along x and y axis
    };

    // external forces applied to the truss
    std::vector<double> externalForces = { 
        0, 0,           // no forces at node 0
        0, 0,           // no forces at node 1
        2, -1,          // no forces at node 2
    };

    // global stiffness matrix declaration
    arma::Mat<double> globalStiffness(dofsCount, dofsCount);

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

    for(size_t i=0; i<elementsCount; i++)
    {
        std::cout << "ELEMENT " << i << std::endl;
        // element's degrees of freenoom matrix
        uint dofs[4] = { topology[i][0], topology[i][1], topology[i][2], topology[i][3] };

        // extract element's begin and end coordinates
        double x1 = coordinates[dofs[0]];
        double y1 = coordinates[dofs[1]];
        double x2 = coordinates[dofs[2]];
        double y2 = coordinates[dofs[3]];
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
        double k = (A * E) / length;
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
    arma::Col<double> globalForces(externalForces);
    std::cout << "Global forces vector: " << std::endl;
    std::cout << globalForces << std::endl;

    // global stiffness matrix and load vector reduction according to given constrains
    arma::Mat<double> reducedStiffness = globalStiffness;
    arma::Col<double> reducedForces = globalForces;

    for(int i=dofsCount-1; i>=0; i--)
    {
        if(constrains[i])
        {
            reducedStiffness.shed_col(i);
            reducedStiffness.shed_row(i);
            reducedForces.shed_row(i);
        }
    }

    std::cout << "Reduced stiffness matrix:" << std::endl;
    std::cout << reducedStiffness << std::endl;

    std::cout << "Reduced loads' vector:" << std::endl;
    std::cout << reducedForces << std::endl; 

    // reduced displacements' vector
    arma::Col<double> reducedDisplacements(dofsCount);
    arma::solve(reducedDisplacements, reducedStiffness, reducedForces);
    std::cout << "Reduced displacements' vector: " << std::endl;
    std::cout << reducedDisplacements << std::endl;

    // global displacements' vector
    arma::Col<double> dispalcementsGlobal(dofsCount, arma::fill::zeros);
    
    uint count = 0;
    for(size_t i=0; i<dofsCount; i++)
    {
        if(!constrains[i])
            dispalcementsGlobal(i) = reducedDisplacements(count++);
    }

    std::cout << "Global displacements vector: " << std::endl;
    std::cout << dispalcementsGlobal << std::endl;

    // elements internal forces
    for(size_t i=0; i<elementsCount; i++)
    {
        // degrees of freedom
        uint dofs[4] = { topology[i][0], topology[i][1], topology[i][2], topology[i][3] };

        // element's displacements in global CS
        arma::Col<double> elementDisplacements(4);
        for(size_t j=0; j<4; j++)
        {
            elementDisplacements(j) = dispalcementsGlobal(dofs[j]);
        }

        // elements' internal forces (stress)
        arma::Col<double> elementInternalForces(4);
        elementInternalForces = elementsStiffnessGlobalMatrices.at(i) * elementDisplacements;
        std::cout << "ELEMENT " << i << " internal forces vector: " << std::endl;
        std::cout << elementInternalForces << std::endl;
    }

    return 0;
}