#ifndef __DRAWER_H__
#define __DRAWER_H__

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
    svg::Document document;

    uint scale = 4;
    const double offset = 100;
    const double constrainOffset = 6;
    const double forceLineLnegth = 80;
    const double arrowLength = 30;
    const double arrowThickness = 10;
    const double fixSize = 30;

public:
    Drawer(const Truss& truss, std::string fileName);
    void draw();

private:
    svg::Dimensions calculateDimensions();
    void drawNodes();
    void drawElements();
    void drawExternalForces();
    void drawConstrains();
    void drawHorizontalConstrain(double x, double y);
    void drawHorizontalSliderConstrain(double x, double y);
    void drawVerticalConstrain(double x, double y);
    void drawVerticalSliderConstrain(double x, double y);
};

#endif // __DRAWER_H__