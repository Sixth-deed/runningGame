#include "core/ExtendedPhysicsObj.h"

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
double calculateInertia(double mass_, std::vector<gMath::tVector> *shape)
{

    double inertia = 0.0;
    if (shape->size() == 1) // circle
    {
        double radius = shape->at(0).magnitude();
        inertia = mass_ * radius * radius / 2.0;
    }
    else if (shape->size() == 2) // line segment
    {
        double length = shape->at(0).magnitude();
        inertia = mass_ * length * length / 12.0;
    }
    else
    { // Convex polygon
        const size_t v_size = shape->size();
        // 分母
        std::vector<double> parent(v_size);
        double parent_sum = 0;
        for (int i = 0; i < v_size; i++)
        {
            if (i != 0)
            {
                gMath::tVector v1 = shape->at(i - 1);
                gMath::tVector v2 = shape->at(i);
                parent[i] = abs(v1.cross(v2));
                parent_sum += parent[i];
            }
            if (i == 0)
            {
                gMath::tVector v1 = shape->at(v_size - 1);
                gMath::tVector v2 = shape->at(0);
                parent[i] = abs(v1.cross(v2));
                parent_sum += parent[i];
            }
        }
        // 分子
        double child_sum = 0;
        for (int i = 0; i < v_size; i++)
        {
            if (i != 0)
            {
                const gMath::tVector &v1 = shape->at(i - 1);
                const gMath::tVector &v2 = shape->at(i);
                child_sum += parent[i] * (v1.sLen() + v1.dot(v2) + v2.sLen());
            }
            if (i == 0)
            {
                const gMath::tVector &v1 = shape->at(v_size - 1);
                const gMath::tVector &v2 = shape->at(0);
                child_sum += parent[i] * (v1.sLen() + v1.dot(v2) + v2.sLen());
            }
        }

        
        inertia = mass_ * child_sum / (6 * parent_sum);
    }
    return inertia;
}
