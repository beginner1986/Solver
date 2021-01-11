#include <iostream>
#include <cmath>
#include <armadillo>

using namespace std;

int main()
{
    double A = 2;
    double E = 30e6;

    const int nodesCount = 4;
    double coordinates[nodesCount * 2] = { 0, 0, 0, 120, 120, 120, 120, 0 };

    const int elementsCount = 4;
    int topology[elementsCount][4] = {
        { 0, 1, 2, 3 },
        { 2, 3, 4, 5 },
        { 4, 5, 6, 7 },
        { 0, 1, 6, 7 }
    };

    return 0;
}