#include <iostream>
#include <cmath>
#include <armadillo>

using namespace std;

int main()
{
    //matherial properties
    double A = 2;       // elements' cross area
    double E = 1e6;     // Young's modulus

    // nodes coordinates definition (each node x and y) in one table as they are also dofs
    const size_t nodesCount = 4;
    const size_t dofsCount = 2 * nodesCount;
    double coordinates[dofsCount] = { 
        0, 0,       // node 0
        0, 100,     // node 1
        100, 0,     // node 2
        100, 100    // node 3
    };

    // nides connections by the beams
    const size_t elementsCount = 4;
    uint topology[elementsCount][4] = {
        { 2, 3, 6, 7 },     // node 1 to 3
        { 0, 1, 6, 7 },     // node 0 to 3
        { 0, 1, 4, 5 },     // node 0 to 2
        { 4, 5, 6, 7 }      // node 2 to 3
    };

    // external forces applied to the truss
    double externalForces[dofsCount] = { 
        0, 0,       // no forces at node 0
        0, 0,       // no forces at node 1
        0, 0,       // no forces at node 2
        0, -10000   // 10 000 N at node 3 vertical downside
    };

    // global stiffness matrix declaration
    arma::Mat<double> globalStiffness(dofsCount, dofsCount);

    // elements' stiffness matrices
    // template for elements' stiffness matrices
    arma::Mat<double> localStiffness(4, 4, arma::fill::zeros);
    localStiffness(0, 0) = 1;
    localStiffness(0, 2) = -1;
    localStiffness(2, 0) = -1;
    localStiffness(2, 2) = 1;

    for(size_t i=0; i<elementsCount; i++)
    {
        cout << "ELEMENT " << i << endl;
        // element's degrees of freenoom matrix
        uint dofs[4] = { topology[i][0], topology[i][1], topology[i][2], topology[i][3] };

        // extract element's begin and end coordinates
        double x1 = coordinates[dofs[0]];
        double y1 = coordinates[dofs[1]];
        double x2 = coordinates[dofs[2]];
        double y2 = coordinates[dofs[3]];
        cout << "\t(" << x1 << ", " << y1 << ") - (" << x2 << ", " << y2 << ")" << endl;

        // claculate the length nad parameters for trigonometry
        double dx = abs(x2 - x1);
        double dy = abs(y2 - y1);
        double length = sqrt(dx * dx + dy * dy);
        cout << "\tdx = " << dx << "; dy = " << dy << "; l = " << length << endl;

        // cosinus and sinus of the angle between local and global CS
        double cos = dx / length;
        double sin = dy / length;
        cout << "\tcos = " << cos << "; sin =" << sin << endl;

        // beam stiffness value (scalar)
        double k = (A * E) / length;
        cout << "\tk = " << k << endl;

        // element's stfiffness matrix in local CS
        arma::Mat<double> elementStiffnessLocal = localStiffness;
        elementStiffnessLocal *= k;

        cout << "\t element stiffness in local CS:" << endl;
        cout << elementStiffnessLocal << endl;

        // transformation matrix
        arma::Mat<double> transtofrmationMatrix = {
            { cos * cos, cos * sin, -cos * cos, -cos * sin },
            { cos * sin, sin * sin, -cos * sin, -sin * sin },
            { -cos * cos, - cos * sin, cos * cos, cos * sin },
            { -cos * sin, -sin * sin, cos * sin, sin * sin }
        };
        cout << "\tTransformation matrix:" << endl;
        cout << transtofrmationMatrix << endl;

        // calculate element's stiffnesss matrix in global CS
        arma::Mat<double> elementStiffnessGlobal = transtofrmationMatrix * k;

        cout << "\tElement stiffness matrix in global CS:" << endl;
        cout << elementStiffnessGlobal << endl;

        // assembly global stiffness matrix
        for(size_t i=0; i<4; i++)
        {
            for(size_t j=0; j<4; j++)
            {
                globalStiffness(dofs[i], dofs[j]) += elementStiffnessGlobal(i, j);
                //cout << "\tdofs: " << dofs[i] << ", " << dofs[j] << endl;
                //cout << "\tvalue = " << elementStiffnessGlobal(i, j) << endl << endl;
            }
        }
    }

    cout << "Global stiffness matrix: " << endl;
    cout << globalStiffness << endl;

    // global forces vector
    arma::Col<double> globalForces(dofsCount, arma::fill::zeros);

    return 0;
}