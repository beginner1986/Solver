#include "Drawer.h"

Drawer::Drawer(SolvedTruss &truss, std::string fileName) 
    : truss(truss), fileName(fileName)
{
    dimensions = calculateDimensions(truss);
    document = svg::Document(fileName, svg::Layout(dimensions, svg::Layout::BottomLeft));
}

void Drawer::draw(SolvedTruss &truss)
{
    drawNodes(truss, svg::Color::Black);
    drawNodes(truss.getInputTruss(), svg::Color::Purple);
    drawElements(truss, svg::Color::Blue, 3);
    drawElements(truss.getInputTruss(), svg::Color::Magenta, 1);
    drawForces(truss.getInputTruss(), truss.externalForces, svg::Color::Red);
    drawForces(truss, truss.reactionForces, svg::Color::Green);
    drawConstrains(truss.getInputTruss());
    drawInternalStress(truss);

    document.save();
}

svg::Dimensions Drawer::calculateDimensions(const SolvedTruss &truss)
{
    double maxX = 0;
    double maxY = 0;

    for (size_t i = 0; i < truss.dofsCount; i++)
    {
        if (i % 2 != 0)
        {
            double x1 = truss.coordinates.at(i);
            double x2 = truss.getInputTruss().coordinates.at(i);
            maxX = std::max(x1, maxX);
            maxX = std::max(x2, maxX);
        }
        else
        {
            double y1 = truss.coordinates.at(i);
            double y2 = truss.getInputTruss().coordinates.at(i);
            maxY = std::max(y1, maxY);
            maxY = std::max(y2, maxY);
        }
    }

    scale = 1000 / std::max(maxX, maxY);

    maxX *= scale;
    maxY *= scale;

    return svg::Dimensions(maxX + 2 * offset, maxY + 2 * offset);
}

void Drawer::drawNodes(const Truss &truss, svg::Color color)
{
    for (size_t node = 0; node < truss.dofsCount; node += 2)
    {
        double x = truss.coordinates.at(node);
        double y = truss.coordinates.at(node + 1);

        x = x * scale + offset;
        y = y * scale + offset;

        document << svg::Circle(svg::Point(x, y), 10, svg::Fill(color));
    }
}

void Drawer::drawElements(const Truss &truss, svg::Color color, double width)
{
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

        document << svg::Line(svg::Point(x1, y1), svg::Point(x2, y2), svg::Stroke(width, color));
    }
}

void Drawer::drawForces(const Truss &truss, const std::vector<double> forces, svg::Color color)
{
    for (size_t dof = 0; dof < truss.dofsCount; dof++)
    {
        double force = forces.at(dof);

        if (force != 0)
        {
            // force line coordinates - not scaled
            double x2;
            double y2;
            if (dof % 2 == 0)
            {
                x2 = truss.coordinates.at(dof);
                y2 = truss.coordinates.at(dof + 1);
            }
            else
            {
                x2 = truss.coordinates.at(dof - 1);
                y2 = truss.coordinates.at(dof);
            }

            x2 = x2 * scale + offset;
            y2 = y2 * scale + offset;
            double x1 = x2;
            double y1 = y2;

            if (dof % 2 == 0)
                x1 += (force > 0) ? -forceLineLnegth : forceLineLnegth;
            else
                y1 += (force > 0) ? -forceLineLnegth : forceLineLnegth;

            document << svg::Line(svg::Point(x1, y1), svg::Point(x2, y2), svg::Stroke(2, color));

            // arrowhead
            double arrowX1, arrowY1, arrowX2, arrowY2;

            if (dof % 2 == 0) // horizontal
            {
                arrowY1 = y2 + arrowThickness / 2;
                arrowY2 = y2 - arrowThickness / 2;

                if (force > 0) // right
                    arrowX1 = x2 - arrowLength;
                else //left
                    arrowX1 = x2 + arrowLength;

                arrowX2 = arrowX1;
            }
            else // vertical
            {
                arrowX1 = x2 + arrowThickness / 2;
                arrowX2 = x2 - arrowThickness / 2;

                if (force > 0) // up
                    arrowY1 = y2 - arrowLength;
                else // down
                    arrowY1 = y2 + arrowLength;

                arrowY2 = arrowY1;
            }

            svg::Polygon arrow(svg::Fill((svg::Color)(color)));
            arrow << svg::Point(x2, y2) << svg::Point(arrowX1, arrowY1) << svg::Point(arrowX2, arrowY2);
            document << arrow;
        }
    }
}

void Drawer::drawInternalStress(const SolvedTruss &truss) 
{
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

        double dx = x2 - x1;
        double dy = y2 - y1;
        double length = sqrt(dx * dx + dy * dy);
        double cos = dx / length;
        double sin = dy / length;
        dx = abs(dx);
        dy = abs(dy);

        double x;
        double y;

        // arrow 1
        if(x1 < x2)
            x = x1 + (dx / 3);
        else
            x = x1 - (dx / 3);

        if(y1 < y2)
            y = y1 + (dy / 3);
        else
            y = y1 - (dy / 3);

        double arrowX1 = x - arrowLength;
        double arrowY1 = y - arrowThickness;
        // arrow1 point rotation around (x, y)
        double arrowX1rot = (arrowX1 - x) * cos - (arrowY1 - y) * sin + x;
        double arrowY1rot = (arrowX1 - x) * sin + (arrowY1 - y) * cos + y;

        double arrowX2 = x - arrowLength;
        double arrowY2 = y + arrowThickness;
        // arrow2 point rotation around (x, y)
        double arrowX2rot = (arrowX2 - x) * cos - (arrowY2 - y) * sin + x;
        double arrowY2rot = (arrowX2 - x) * sin + (arrowY2 - y) * cos + y;

        svg::Polygon arrow1(svg::Fill((svg::Color::Blue)));
            arrow1 << svg::Point(x, y) 
                << svg::Point(arrowX1rot, arrowY1rot) 
                << svg::Point(arrowX2rot, arrowY2rot) 
                << svg::Point(x, y);
        document << arrow1;

        // arrow 2
        if(x1 < x2)
            x = x2 - (dx / 3);
        else
            x = x2 + (dx / 3);

        if(y1 < y2)
            y = y2 - (dy / 3);
        else
            y = y2 + (dy / 3);

        arrowX1 = x + arrowLength;
        arrowY1 = y - arrowThickness;

        // arrow1 point rotation around (x, y)
        arrowX1rot = (arrowX1 - x) * cos - (arrowY1 - y) * sin + x;
        arrowY1rot = (arrowX1 - x) * sin + (arrowY1 - y) * cos + y;

        arrowX2 = x + arrowLength;
        arrowY2 = y + arrowThickness;
        // arrow2 point rotation around (x, y)
        arrowX2rot = (arrowX2 - x) * cos - (arrowY2 - y) * sin + x;
        arrowY2rot = (arrowX2 - x) * sin + (arrowY2 - y) * cos + y;

        svg::Polygon arrow2(svg::Fill((svg::Color::Blue)));
            arrow2 << svg::Point(x, y) 
                << svg::Point(arrowX1rot, arrowY1rot) 
                << svg::Point(arrowX2rot, arrowY2rot) 
                << svg::Point(x, y);
        document << arrow2;
    }
}

void Drawer::drawConstrains(const Truss &truss)
{
    for (size_t dof = 0; dof < truss.dofsCount; dof++)
    {
        if (truss.constrains.at(dof))
        {
            double x, y;

            if (dof % 2 == 0) // x axis
            {
                x = truss.coordinates.at(dof);
                y = truss.coordinates.at(dof + 1);

                if (truss.constrains.at(dof + 1))
                {
                    drawVerticalConstrain(x, y);
                    dof++;
                }
                else
                {
                    drawHorizontalSliderConstrain(x, y);
                }
            }
            else // y axis
            {
                x = truss.coordinates.at(dof - 1);
                y = truss.coordinates.at(dof);
                drawVerticalSliderConstrain(x, y);
            }
        }
    }
}

void Drawer::drawHorizontalConstrain(double x, double y)
{
    x = x * scale + offset;
    y = y * scale + offset;

    double direction = 1;
    if (x < (dimensions.width / 2))
        direction = -1;

    x = x + constrainOffset * direction;

    // triangle with underline
    svg::Polyline fix(svg::Stroke(2, svg::Color::Green));
    fix << svg::Point(x + direction * (fixSize / 2), y - fixSize / 2)
        << svg::Point(x, y)
        << svg::Point(x + direction * (fixSize / 2), y + fixSize / 2)
        << svg::Point(x + direction * (fixSize / 2), y + fixSize * 3 / 4)
        << svg::Point(x + direction * (fixSize / 2), y - fixSize * 3 / 4);
    document << fix;

    // diagonal lines
    document << svg::Line(
        svg::Point(x + direction * (fixSize / 2), y + fixSize * 2 / 5),
        svg::Point(x + direction * fixSize, y + fixSize / 5),
        svg::Stroke(2, svg::Color::Green));
    document << svg::Line(
        svg::Point(x + direction * (fixSize / 2), y),
        svg::Point(x + direction * fixSize, y - fixSize / 5),
        svg::Stroke(2, svg::Color::Green));
    document << svg::Line(
        svg::Point(x + direction * (fixSize / 2), y - fixSize * 2 / 5),
        svg::Point(x + direction * fixSize, y - fixSize * 3 / 5),
        svg::Stroke(2, svg::Color::Green));
}

void Drawer::drawHorizontalSliderConstrain(double x, double y)
{
    x = x * scale + offset;
    y = y * scale + offset;

    double direction = 1;
    if (x < (dimensions.width / 2))
        direction = -1;

    x = x + constrainOffset * direction;

    // triangle with underline
    svg::Polyline fix(svg::Stroke(2, svg::Color::Green));
    fix << svg::Point(x + direction * (fixSize / 2), y - fixSize / 2)
        << svg::Point(x, y)
        << svg::Point(x + direction * (fixSize / 2), y + fixSize / 2)
        << svg::Point(x + direction * (fixSize / 2), y + fixSize * 3 / 4)
        << svg::Point(x + direction * (fixSize / 2), y - fixSize * 3 / 4);
    document << fix;

    // second underline
    document << svg::Line(
        svg::Point(x + direction * (fixSize * 2 / 3), y + fixSize * 3 / 4),
        svg::Point(x + direction * (fixSize * 2 / 3), y - fixSize * 3 / 4),
        svg::Stroke(2, svg::Color::Green)
    );

    // diagonal lines
    document << svg::Line(
        svg::Point(x + direction * (fixSize * 2 / 3), y + fixSize * 2 / 5),
        svg::Point(x + direction * fixSize, y + fixSize / 5),
        svg::Stroke(2, svg::Color::Green)
    );
    document << svg::Line(
        svg::Point(x + direction * (fixSize * 2 / 3), y),
        svg::Point(x + direction * fixSize, y - fixSize / 5),
        svg::Stroke(2, svg::Color::Green)
    );
    document << svg::Line(
        svg::Point(x + direction * (fixSize * 2 / 3), y - fixSize * 2 / 5),
        svg::Point(x + direction * fixSize, y - fixSize * 3 / 5),
        svg::Stroke(2, svg::Color::Green)
    );
}

void Drawer::drawVerticalConstrain(double x, double y)
{
    x = x * scale + offset;
    y = y * scale + offset - constrainOffset;

    // triangle with underline
    svg::Polyline fix(svg::Stroke(2, svg::Color::Green));
    fix << svg::Point(x - fixSize / 2, y - fixSize / 2)
        << svg::Point(x, y)
        << svg::Point(x + fixSize / 2, y - fixSize / 2)
        << svg::Point(x + fixSize * 3 / 4, y - fixSize / 2)
        << svg::Point(x - fixSize * 3 / 4, y - fixSize / 2);
    document << fix;

    // diagonal lines
    document << svg::Line(
        svg::Point(x - fixSize * 3 / 5, y - fixSize),
        svg::Point(x - fixSize * 2 / 5, y - fixSize / 2),
        svg::Stroke(2, svg::Color::Green));
    document << svg::Line(
        svg::Point(x - fixSize / 5, y - fixSize),
        svg::Point(x, y - fixSize / 2),
        svg::Stroke(2, svg::Color::Green));
    document << svg::Line(
        svg::Point(x + fixSize * 1 / 5, y - fixSize),
        svg::Point(x + fixSize * 2 / 5, y - fixSize / 2),
        svg::Stroke(2, svg::Color::Green));
}

void Drawer::drawVerticalSliderConstrain(double x, double y)
{
    x = x * scale + offset;
    y = y * scale + offset - constrainOffset;

    // triangle with underline
    svg::Polyline fix(svg::Stroke(2, svg::Color::Green));
    fix << svg::Point(x - fixSize / 2, y - fixSize / 2)
        << svg::Point(x, y)
        << svg::Point(x + fixSize / 2, y - fixSize / 2)
        << svg::Point(x + fixSize * 3 / 4, y - fixSize / 2)
        << svg::Point(x - fixSize * 3 / 4, y - fixSize / 2);
    document << fix;

    // second underline
    document << svg::Line(
        svg::Point(x - fixSize * 3 / 4, y - fixSize * 2 / 3),
        svg::Point(x + fixSize * 3 / 4, y - fixSize * 2 / 3),
        svg::Stroke(2, svg::Color::Green)
    );

    // diagonal lines
    document << svg::Line(
        svg::Point(x - fixSize * 3 / 5, y - fixSize),
        svg::Point(x - fixSize * 2 / 5, y - fixSize * 2 / 3),
        svg::Stroke(2, svg::Color::Green));
    document << svg::Line(
        svg::Point(x - fixSize / 5, y - fixSize),
        svg::Point(x, y - fixSize * 2 / 3),
        svg::Stroke(2, svg::Color::Green));
    document << svg::Line(
        svg::Point(x + fixSize * 1 / 5, y - fixSize),
        svg::Point(x + fixSize * 2 / 5, y - fixSize * 2 / 3),
        svg::Stroke(2, svg::Color::Green));
}
