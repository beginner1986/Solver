#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <algorithm>
#include <cmath>
#include <armadillo>

#include "lib/simple_svg_1.0.0.hpp"
#include "SolvedTruss.h"

class Drawer
{
private:
    SolvedTruss &truss;
    std::string fileName;
    svg::Dimensions dimensions;
    svg::Document document;

    uint scale = 4;
    const double offset = 100;
    const double constrainOffset = 6;
    const double forceLineLnegth = 80;
    const double arrowLength = 30;
    const double arrowThickness = 10;
    const double fixSize = 30;

public:
    Drawer(SolvedTruss &truss, std::string fileName);
    void draw();

private:
    svg::Dimensions calculateDimensions(Truss &truss);
    void drawNodes(Truss &truss);
    void drawElements(Truss &truss);
    void drawExternalForces(Truss &truss);
    void drawConstrains(Truss &truss);
    void drawHorizontalConstrain(double x, double y);
    void drawHorizontalSliderConstrain(double x, double y);
    void drawVerticalConstrain(double x, double y);
    void drawVerticalSliderConstrain(double x, double y);
};

#endif // __DRAWER_H__