#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <sstream>
#include <algorithm>
#include <cmath>
#include <armadillo>

#include "lib/simple_svg_1.0.0.hpp"
#include "Truss.h"

class Drawer
{
private:
    const Truss& truss;
    std::string fileName;

    const double offset = 100;
    const uint scale = 4;
    const double forceLineLnegth = 80;
    const double arrowLength = 30;
    const double arrowThickness = 10;
    const double fixSize = 30;

public:
    Drawer(const Truss& truss, std::string fileName);

private:
    svg::Dimensions calculateDimensions();
};

#endif // __DRAWER_H__