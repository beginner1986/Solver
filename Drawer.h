#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <algorithm>
#include <armadillo>

#include "lib/simple_svg_1.0.0.hpp"
#include "Truss.h"

class Drawer
{
private:
    const Truss& truss;
    std::string fileName;

    const double offset = 50;
    const uint scale = 4;
    const double forceLineLnegth = 40;

public:
    Drawer(const Truss& truss, std::string fileName);

private:
    svg::Dimensions calculateDimensions();
};

#endif // __DRAWER_H__