#include "Drawer.h"

Drawer::Drawer(const Truss &truss, std::string fileName) : truss(truss), fileName(fileName)
{
    svg::Dimensions dimensions = calculateDimensions();
    svg::Document document(fileName, svg::Layout(dimensions, svg::Layout::BottomLeft));

    for (size_t node = 0; node < truss.dofsCount; node += 2)
    {
        double x = truss.coordinates.at(node);
        double y = truss.coordinates.at(node + 1);

        x = x * scale + offset;
        y = y * scale + offset;

        document << svg::Circle(svg::Point(x, y), 5, svg::Fill(svg::Color::Black));
    }

    document.save();
}

svg::Dimensions Drawer::calculateDimensions()
{
    double maxX = 0;
    double maxY = 0;

    for (size_t i = 0; i < truss.dofsCount; i++)
    {
        if (i % 2 != 0)
        {
            double x = truss.coordinates.at(i);
            maxX = std::max(x, maxX);
        }
        else
        {
            double y = truss.coordinates.at(i);
            maxY = std::max(y, maxY);
        }
    }

    maxX *= scale;
    maxY *= scale;

    return svg::Dimensions(maxX + 2 * offset, maxY + 2 * offset);
}
