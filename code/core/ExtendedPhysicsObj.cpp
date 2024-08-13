#include "ExtendedPhysicsObj.h"

/**
 * Calculates the inertia of a shape based on its mass and geometry.
 *
 * The shape can be one of three types: circle, line segment, or convex polygon.
 * The inertia is calculated differently for each shape type.
 *
 * @param mass_ the mass of the shape
 * @param shape a pointer to a vector of gMath::tVector objects representing the shape's geometry
 *
 * @return the calculated inertia of the shape
 */
double caculateInertia(double mass_, std::vector<gMath::tVector>* shape)
{

    double inertia = 0.0;
    if (shape->size() == 1) //circle
    {
        double radius = shape->at(0).magnitude();
        inertia = mass_ * radius * radius / 2.0;
    }
    else if (shape->size() == 2) //line segment
    {
        double length = shape->at(0).magnitude();
        inertia = mass_ * length * length / 12.0;
    }
    else { // Convex polygon
        double area = 0.0;
        double inertiaXX = 0.0;
        double inertiaYY = 0.0;

        for (size_t i = 0; i < shape->size(); i++) {
            size_t next = (i + 1) % shape->size();
            double x1 = shape->at(i).x;
            double y1 = shape->at(i).y;
            double x2 = shape->at(next).x;
            double y2 = shape->at(next).y;

            double crossProduct = x1 * y2 - x2 * y1;
            area += crossProduct;

            inertiaXX += (x1 * x1 + x2 * x1 + x2 * x2) * crossProduct;
            inertiaYY += (y1 * y1 + y2 * y1 + y2 * y2) * crossProduct;
        }

        area = fabs(area) / 2.0;
        inertiaXX = (inertiaXX / 12.0);
        inertiaYY = (inertiaYY / 12.0);

        inertia = mass_ * (inertiaXX + inertiaYY);
    }
    return inertia;
}
