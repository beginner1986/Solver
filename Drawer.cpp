#include "Drawer.h"

Drawer::Drawer(const Truss &truss, std::string fileName) : truss(truss), fileName(fileName)
{
    svg::Dimensions dimensions = calculateDimensions();
    svg::Document document(fileName, svg::Layout(dimensions, svg::Layout::BottomLeft));

    // draw the nodes
    for (size_t node = 0; node < truss.dofsCount; node += 2)
    {
        double x = truss.coordinates.at(node);
        double y = truss.coordinates.at(node + 1);

        x = x * scale + offset;
        y = y * scale + offset;

        document << svg::Circle(svg::Point(x, y), 10, svg::Fill(svg::Color::Black));
    }

    // draw the elements
    for (size_t element = 0; element < truss.elementsCount; element++)
    {
        arma::Row<uint> dofs = truss.topology.row(element);
        double x1 = truss.coordinates.at(dofs(0));
        double y1 = truss.coordinates.at(dofs(1));
        double x2 = truss.coordinates.at(dofs(2));
        double y2 = truss.coordinates.at(dofs(3));

        x1 = x1 * scale + offset;
        y1 = y1 * scale + offset;
        x2 = x2 * scale + offset;
        y2 = y2 * scale + offset;

        document << svg::Line(svg::Point(x1, y1), svg::Point(x2, y2), svg::Stroke(2, svg::Color::Blue));
    }

    // draw external forces
    for (size_t dof = 0; dof < truss.dofsCount; dof++)
    {
        double force = truss.externalForces.at(dof);

        if(force != 0)
        {
            // force line coordinates - not scaled
            double x2 = truss.coordinates.at(dof);
            double y2 = truss.coordinates.at(dof);
            x2 = x2 * scale + offset;
            y2 = y2 * scale + offset;
            double x1 = x2;
            double y1 = y2;

            if(dof % 2 == 0)
                x1 += (force > 0) ? -forceLineLnegth : forceLineLnegth;
            else
                y1 += (force > 0) ? -forceLineLnegth : forceLineLnegth;

            document << svg::Line(svg::Point(x1, y1), svg::Point(x2, y2), svg::Stroke(2, svg::Color::Red));

            // arrowhead
            double arrowX1, arrowY1, arrowX2, arrowY2;

            if(dof % 2 == 0)
            {
                arrowY1 = y2 + arrowThickness / 2;  
                arrowY2 = y2 - arrowThickness / 2;

                if(force > 0)
                    arrowX1 = x2 - arrowLength;
                else
                    arrowX1 = x2 + arrowLength;
                
                arrowX2 = arrowX1;
            }
            else
            {
                arrowX1 = x2 + arrowThickness / 2;
                arrowX2 = x2 - arrowThickness / 2;

                if(force > 0)
                    arrowY1 = y2 - arrowLength;
                else
                    arrowY1 = y2 + arrowLength;
                
                arrowY2 = arrowY1;
            }

            svg::Polygon arrow(svg::Fill(svg::Color::Red));
            arrow << svg::Point(x2, y2) << svg::Point(arrowX1, arrowY1) << svg::Point(arrowX2, arrowY2);
            document << arrow;
        }
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
