#include <iostream>
#include <cmath>
#include <armadillo>

using namespace std;

int main()
{
    //matherial properties
    double A = 2;
    double E = 30e6;

    // nodes coordinates definition
    const size_t nodesCount = 4;
    const size_t dofsCount = 2 * nodesCount;
    double coordinates[dofsCount] = { 0, 0, 0, 120, 120, 120, 120, 0 };

    // nides connections by the beams
    const size_t elementsCount = 4;
    int topology[elementsCount][4] = {
        { 0, 1, 2, 3 },
        { 2, 3, 4, 5 },
        { 4, 5, 6, 7 },
        { 0, 1, 6, 7 }
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
        // extract element's nodes indexes
        size_t x1Index = topology[i][0];
        size_t y1Index = topology[i][1];
        size_t x2Index = topology[i][2];
        size_t y2Index = topology[i][3];

        // extract element's begin and end coordinates
        double x1 = coordinates[x1Index];
        double x2 = coordinates[y1Index];
        double y1 = coordinates[x2Index];
        double y2 = coordinates[y2Index];

        // claculate the length nad parameters for trigonometry
        double dx = abs(x2 - x1);
        double dy = abs(y2 - y1);
        double length = sqrt(dx * dx + dy * dy);

        // cosinus and sinus of the angle between local and global CS
        double cos = dx / length;
        double sin = dy / length;

        // beam stiffness value (scalar)
        double k = (A * E) / length;

        // element's stfiffness matrix in local CS
        arma::Mat<double> elementStiffnessLocal = k * localStiffness;

        // transformation matrix
        arma::Mat<double> transtofrmationMatrix = {
            { cos * cos, cos * sin, -cos * cos, -cos * sin },
            { cos * sin, sin * sin, -cos * sin, -sin * sin },
            { -cos * cos, - cos * sin, cos * cos, cos * sin },
            { -cos * sin, -sin * sin, cos * sin, sin * sin }
        };

        // calculate element's stiffnesss matrix in global CS
        arma::Mat<double> elementStiffnessGlobal = k * transtofrmationMatrix;
    }

    return 0;
}