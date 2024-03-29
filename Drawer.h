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

    uint scale;
    const double offset = 100;
    const double constrainOffset = 6;
    const double forceLineLnegth = 80;
    const double arrowLength = 30;
    const double arrowThickness = 10;
    const double fixSize = 30;

public:
    Drawer(SolvedTruss &truss, std::string fileName);
    void draw(SolvedTruss &truss);

private:
    svg::Dimensions calculateDimensions(const SolvedTruss &truss);
    void drawNodes(const Truss &truss, svg::Color color);
    void drawElements(const Truss &truss, svg::Color color, double width);
    void drawForces(const Truss &truss, const std::vector<double> forces, svg::Color color);
    void drawInternalStress(const SolvedTruss &truss);
    void drawConstrains(const Truss &truss);
    void drawHorizontalConstrain(double x, double y);
    void drawHorizontalSliderConstrain(double x, double y);
    void drawVerticalConstrain(double x, double y);
    void drawVerticalSliderConstrain(double x, double y);
    void rotatePoint(const double centerX, const double centerY, double &pointX, double &pointY, const double sin, const double cos);
    void drawArrowhead(const double x, const double y, const double sin, const double cos, bool reversed = false);
};

#endif // __DRAWER_H__