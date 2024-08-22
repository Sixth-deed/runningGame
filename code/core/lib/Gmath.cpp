#include "Gmath.h"

using namespace gMath;
Projection projectTo( const std::vector<tVector>& vectors,const tVector &axis) 
{
    switch (vectors.size())
    {
    case 1:
        const tVector r = axis * vectors[0].x;
        return gMath::Projection(-vectors[0].x, r.reverse(), vectors[0].x, std::move(r));
    case 2:
        double t = vectors[0].dot(axis);
        const tVector v = axis * t;
        if (t > 0.0)
            return gMath::Projection(-t, v.reverse(), t, std::move(v));
        return gMath::Projection(t, std::move(v), -t, v.reverse());
    default:
        // 考虑到是顺时针遍历顶点可以稍作优化
        double max = vectors[0].dot(axis), min;
        int i = 1;
        const tVector *minpt, *maxpt;
        for (; i < vectors.size(); i++)
        {
            double projection_i = vectors[i].dot(axis);
            if (projection_i > max)
            {
                max = projection_i;
            }
            else
            {
                maxpt = &vectors[i - 1];
                min = projection_i;
                break;
            }
        }
        for (; i < vectors.size(); i++)
        {
            double projection_i = vectors[i].dot(axis);
            if (projection_i < min)
            {
                min = projection_i;
            }
            else
            {
                minpt = &vectors[i - 1];
                break;
            }
        }

        return gMath::Projection(min, tVector(*minpt), max, tVector(*maxpt));
    }
}
bool gMath::mRectangle::Inside(const Crdinate &crd, const std::vector<tVector> *vs) const
{
    Projection p = projectTo(*vs, tVector(1.0, 0.0));
    if (crd.get_x() + p.low <= l || crd.get_x() + p.high >= r)
        return false;
    Projection p2 =projectTo(*vs, tVector(0.0, 1.0));
    if (crd.get_y() + p.low <= b || crd.get_y() + p.high >= r)
        return false;
    return true;
}