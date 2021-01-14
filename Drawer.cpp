#include "Drawer.h"

Drawer::Drawer()
{
    using namespace svg;
    
    Dimensions dimensions(150, 100);
    Document document("test.svg", Layout(dimensions, Layout::BottomLeft));

    document << Line(Point(10, 10), Point(dimensions.width - 10, dimensions.height - 10), Stroke(1, Color::Black));

    document.save();
}
