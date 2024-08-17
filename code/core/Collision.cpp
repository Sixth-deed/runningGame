#include "Collision.h"
#include <cfloat>
#include <cmath>
#include <unordered_set>
#include <utility>
#include <execution>

using namespace clsn;
gMath::Projection CollisionBox::projectTo(const tVector &axis) const
{
    switch (vectors.size())
    {
    case 1:
        const tVector r = axis * radius();
        return gMath::Projection(-radius(), r.reverse(), radius(), std::move(r));
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

void clsn::CollisionBox::RotateTo(const Angle &angle)
{
    switch (vectors.size())
    {
    case 1:

        return;
    case 2:
        auto rotateAngle = angle - nAngle;
        vectors[0].rotate(rotateAngle);
        ns->at(0).rotate(rotateAngle);
        return;

    case 3:
        auto rotateAngle = angle - nAngle;
        std::for_each(std::execution::par, vectors.begin(), vectors.end(), [&](tVector &vec)
                      { vec.rotate(rotateAngle); });

        std::for_each(std::execution::par, ns->begin(), ns->end(), [&](tVector &vec)
                      {
                vec.rotate(rotateAngle);
                vec.unify(); });
    }
}
mOptional<CollisionLocal> clsn::isReallyIntersects(const Crdinate &crd1, CollisionBox &b1, const Angle &angle1, const Crdinate &crd2, CollisionBox &b2, const Angle &angle2)
{
    //SAT算法
    //GJK - EPA


    // 由对象1的中心点指向对象2的中心点的向量
    tVector offset12 = crd2 - crd1;
    auto consolveLineAndCircle = [] (const tVector &offset, const CollisionBox &Line, const CollisionBox &Circle, bool reversed)
        -> mOptional<CollisionLocal>
    {
        // 不考虑线段两头撞上圆的情形，本身线段只是作辅助的补充碰撞箱
        std::vector<tVector> ns;
        ns.emplace_back(offset + Line.vectors[0]);
        ns.emplace_back(offset - Line.vectors[0]);
        for (const auto &axis : ns)
        {
            auto p1 = Line.projectTo(axis);
            auto p2 = Circle.projectTo(axis);
            p2.addOffset(offset.dot(axis));
            if (p1.high < p2.low || p2.high < p1.low)
            {
                return mOptional<CollisionLocal>();
            }
        }
        // 指向圆心的向量
        const tVector &n12 = Line.ns->at(0).keepDirectionWith(offset);

        double len = Circle.radius() - std::abs(n12.dot(offset));

        const tVector minDvector(len, n12);
        const tVector v2 = (n12 * Circle.radius()).reverse();
        const tVector t = n12.cross(1.0);
        if (reversed)
            return mOptional<CollisionLocal>(CollisionLocal(v2, offset + v2 + minDvector, n12.reverse(), t.reverse()));
        return mOptional<CollisionLocal>(CollisionLocal(offset + v2 + minDvector, v2, n12, t));
    };
    auto consolveLineAndPolygon = [](const tVector &offset, const CollisionBox &Line, const CollisionBox &Polygon, const bool reversed)
        -> mOptional<CollisionLocal>
    {
        // 不考虑线段两头撞上多边形的情形，本身线段只是作辅助的补充碰撞箱
        const tVector n = Line.ns->at(0).keepDirectionWith(offset);

        gMath::Projection p = Polygon.projectTo(n);

        p.addOffset(offset.reverse().dot(n));
        if (p.high < 0 || p.low > 0)
        {
            return mOptional<CollisionLocal>();
        }
        // 确认在线段上相交
        // 通过确认直线与多边形的交点，判断是否在线段上相交
        const tVector t = n.normal().keepDirectionWith(offset);
        // 由于是顺时针遍历，只需要找到第一个从与法向量点乘为负值到与法向量点乘为正值的变化的点
        // 这里是有限状态机
        bool flag = false;
        const double lambda = -offset.dot(n);
        for (int i = 0; i < Polygon.vectors.size(); i++)
        {
            if (Polygon.vectors[i].dot(n) <= lambda)
            {
                flag = true;
                continue;
            }
            else if (flag)
            {
                const double x1 = Polygon.vectors[i - 1].x;
                const double x2 = Polygon.vectors[i].x;
                const double y1 = Polygon.vectors[i - 1].y;
                const double y2 = Polygon.vectors[i].y;
                if ((lambda * (x1 - x2) + x2 * y1 - x1 * y2) / (y1 - y2) + offset.dot(t) - Line.vectors[0].magnitude() > 0)
                {
                    return mOptional<CollisionLocal>();
                }
                else
                {
                    break;
                }
            }
        }
        if (reversed)
            return mOptional<CollisionLocal>(CollisionLocal(p.toLowPoint, offset + p.toLowPoint + n * p.low, n.reverse(), t.reverse()));
        return mOptional<CollisionLocal>(CollisionLocal(offset + p.toLowPoint + n * p.low, p.toLowPoint, n, t));
    };

    auto consolveCircleAndPolygon = [](const tVector &offset, const CollisionBox &Circle, const CollisionBox &Polygon, const bool reversed)
        -> mOptional<CollisionLocal>
    {
        // 进行SAT检测
        // 记录最小位移向量
        const tVector tAxis = offset.getUnifiedV(), *minV = &tAxis;
        auto p1 = Circle.projectTo(tAxis);
        auto p2 = Polygon.projectTo(tAxis);
        p2.addOffset(offset.dot(tAxis));
        if (p1.high < p2.low || p2.high < p1.low)
        {
            return mOptional<CollisionLocal>();
        }
        double mMin;
        const tVector *v1, *v2;
        if (p2.high > p1.high)
        {
            mMin = p1.high - p2.low;
            v1 = &p1.toHighPoint;
            v2 = &p2.toLowPoint;
        }
        else
        {
            mMin = p2.high - p1.low;
            v1 = &p2.toHighPoint;
            v2 = &p1.toLowPoint;
        }

        for (const auto &axis : *(Polygon.ns))
        {
            auto p1 = Circle.projectTo(axis);
            auto p2 = Polygon.projectTo(axis);
            p2.addOffset(offset.dot(axis));
            if (p1.high < p2.low || p2.high < p1.low)
            {
                return mOptional<CollisionLocal>();
            }
            if (p2.high > p1.high)
            {
                const double temp = p1.high - p2.low;
                if (mMin > temp)
                {
                    v1 = &p1.toHighPoint;
                    v2 = &p2.toLowPoint;
                    mMin = temp;
                    minV = &axis;
                }
            }
            else
            {
                const double temp = p2.high - p1.low;
                if (mMin > temp)
                {
                    v1 = &p2.toHighPoint;
                    v2 = &p1.toLowPoint;
                    mMin = temp;
                    minV = &axis;
                }
            }
        }
        const tVector normal = minV->getUnifiedV();
        const tVector t = normal.normal().keepDirectionWith(offset);
        if (reversed)
            return mOptional<CollisionLocal>(CollisionLocal(*v2, *v1, normal.reverse(), t.reverse()));
        return mOptional<CollisionLocal>(CollisionLocal(*v1, *v2, normal, t));
    };
    auto consolveLines = [](const tVector &offset, const CollisionBox &vecA, const CollisionBox &vecB)
        -> mOptional<CollisionLocal>
    {
        // 这里只检测是否相撞，不反回有效的最小分离量
        tVector axis = offset.getUnifiedV();
        auto p1 = vecA.projectTo(axis);
        auto p2 = vecB.projectTo(axis);
        p2.addOffset(offset.dot(axis));
        if (p1.high < p2.low || p2.high < p1.low)
        {
            return mOptional<CollisionLocal>();
        }
        return mOptional<CollisionLocal>(true);
    };
    auto consolveCircles = [](const tVector &offset, const CollisionBox &Ca, const CollisionBox &Cb)
        -> mOptional<CollisionLocal>
    {
        double len = sqrt(offset.sLen());
        if (Ca.radius() + Cb.radius() > len)
        {
            return mOptional<CollisionLocal>();
        }
        const tVector normal = offset.getUnifiedV();
        const tVector t = normal.normal().keepDirectionWith(offset);
        return mOptional<CollisionLocal>(CollisionLocal(normal * Ca.radius(), (normal * Cb.radius()).reverse(), normal, t));
    };

    // 上面为局部函数声明
    // 检测是否要旋转
    if (angle1 != b1.nAngle)
    {
        b1.RotateTo(angle1);
    }
    if (angle2 != b2.nAngle)
    {
        b2.RotateTo(angle2);
    }
    // 枚举碰撞类型
    if (b1.vectors.size() == 1)
    {
        if (b2.vectors.size() == 1)

            return consolveCircles(offset12, b1, b2);

        if (b2.vectors.size() == 2)

            return consolveLineAndCircle(offset12.reverse(), b2, b1, true);

        else
            return consolveCircleAndPolygon(offset12, b1, b2, false);
    }
    else if (b1.vectors.size() == 2)
    {
        if (b2.vectors.size() == 1)
            return consolveLineAndCircle(offset12, b1, b2, false);
        if (b2.vectors.size() == 2)
            return consolveLines(offset12, b1, b2);
        else
            return consolveLineAndPolygon(offset12, b1, b2, false);
    }
    else
    {
        if (b2.vectors.size() == 1)

            return consolveCircleAndPolygon(offset12.reverse(), b2, b1, true);

        if (b2.vectors.size() == 2)

            return consolveLineAndPolygon(offset12.reverse(), b2, b1, true);

        // 多边形与多边形
        else
        {

            // 进行SAT检测
            // 记录最小位移向量
            const tVector *v1, *v2, *minV;
            double mMin = DBL_MAX;
            for (const auto &axis : *(b1.ns))
            {
                auto p1 = b1.projectTo(axis);
                auto p2 = b2.projectTo(axis);
                p2.addOffset(offset12.dot(axis));
                if (p1.high < p2.low || p2.high < p1.low)
                {
                    return mOptional<CollisionLocal>();
                }
                if (p2.high > p1.high)
                {
                    const double temp = p1.high - p2.low;
                    if (mMin > temp)
                    {
                        v1 = &p1.toHighPoint;
                        v2 = &p2.toLowPoint;
                        mMin = temp;
                        minV = &axis;
                    }
                }
                else
                {
                    const double temp = p2.high - p1.low;
                    if (mMin > temp)
                    {
                        v1 = &p2.toHighPoint;
                        v2 = &p1.toLowPoint;
                        mMin = temp;
                        minV = &axis;
                    }
                }
            }
            for (const auto &axis : *(b2.ns))
            {
                auto p1 = b1.projectTo(axis);
                auto p2 = b2.projectTo(axis);
                p2.addOffset(offset12.dot(axis));
                if (p1.high < p2.low || p2.high < p1.low)
                {
                    return mOptional<CollisionLocal>();
                }
                if (p2.high > p1.high)
                {
                    const double temp = p1.high - p2.low;
                    if (mMin > temp)
                    {
                        v1 = &p1.toHighPoint;
                        v2 = &p2.toLowPoint;
                        mMin = temp;
                        minV = &axis;
                    }
                }
                else
                {
                    const double temp = p2.high - p1.low;
                    if (mMin > temp)
                    {
                        v1 = &p2.toHighPoint;
                        v2 = &p1.toLowPoint;
                        mMin = temp;
                        minV = &axis;
                    }
                }
            }
            return mOptional<CollisionLocal>(CollisionLocal(*v1, *v2, *minV, *minV));
        }
    }
}

CollisionBox::CollisionBox(std::vector<tVector> *array, bool rotatable_ = false, const gMath::Angle &angle) : vectors(*array), vectors_p(array), ns(nullptr), nAngle(angle), rotatable(rotatable_)
{

    switch (vectors.size())
    {
    case 0:
        // 抛出异常，不合法的碰撞箱构造
        break;
    // 圆
    case 1:
        b = l = -vectors[0].x;
        t = r = vectors[0].x;

        break;
    // 线段
    case 2:
        if (rotatable)
        {
            double len = std::sqrt(vectors[0].sLen());
            l = b = -len;
            r = t = len;
        }
        else
        {
            l = vectors[0].x > 0 ? (r = vectors[0].x, -vectors[0].x) : (r = -vectors[0].x, vectors[0].x);
            b = vectors[0].y > 0 ? (t = vectors[0].y, -vectors[0].y) : (t = -vectors[0].y, vectors[0].y);
        }
        ns = new std::vector<tVector>{vectors[0].normal_and_unify()};
        break;
    default:
        if (rotatable)
        {
            std::unordered_set<tVector> tempSet;
            double tempMaxLen = vectors[0].sLen();
            tempSet.insert(vectors[0].normal_and_unify());
            for (int i = 1; i < vectors.size(); i++)
            {
                tempMaxLen = std::max(vectors[i].sLen(), tempMaxLen);
                tempSet.insert(vectors[i].normal_and_unify());
            }
            tempMaxLen = std::sqrt(tempMaxLen);
            b = l = static_cast<axisV>(-tempMaxLen);
            t = r = static_cast<axisV>(tempMaxLen);
            ns = new std::vector<tVector>(tempSet.begin(), tempSet.end());
            if (std::abs(angle.getDegrees()) > 1)
            {
                RotateTo(angle);
            }
        }
        else
        {
            std::unordered_set<tVector> tempSet;
            tempSet.insert(vectors[0].normal_and_unify());
            double tl = vectors[0].x, tr = tl, tb = vectors[0].y, tt = tb;
            for (auto vec : vectors)
            {
                tl = std::min(tl, vec.x);
                tr = std::max(tr, vec.x);
                tb = std::min(tb, vec.y);
                tt = std::max(tt, vec.y);
                tempSet.insert(vec.normal_and_unify());
            }
            ns = new std::vector<tVector>(tempSet.begin(), tempSet.end());
            l = static_cast<axisV>(tl);
            r = static_cast<axisV>(tr);
            b = static_cast<axisV>(tb);
            t = static_cast<axisV>(tt);
        }
    }
}
