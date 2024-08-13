#include "Collision.h"
#include <cfloat>
#include <cmath>
#include <unordered_set>
#include <execution>
using namespace clsn;
CollisionBox::Projection CollisionBox::projectTo(const tVector &axis) const
{
    switch (vectors.size())
    {
    case 1:
        return {-radius(), radius()};
    case 2:
        double t = vectors[0].dot(axis);
        if (t > 0.0)
            return {-t, t};
        return {t, -t};
    default:
        double max, min = max = vectors[0].dot(axis);
        for (int i = 1; i < vectors.size(); i++)
        {
            double projection_i = vectors[i].dot(axis);
            min = std::min(min, projection_i);
            max = std::max(max, projection_i);
        }
        return {min, max};
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


tVector clsn::isReallyIntersects(const Crdinate &crd1, CollisionBox &b1, const Angle &angle1, const Crdinate &crd2, CollisionBox &b2, const Angle &angle2)
{
    // 由对象1的中心点指向对象2的中心点的向量
    tVector offset12 = crd2 - crd1;
    auto consolveLineAndCircle = [](const tVector &offset, const CollisionBox &Line, const CollisionBox &Circle) -> tVector
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
                return tVector(0.0, 0.0);
            }
        }
        double len = Circle.radius() - std::abs(Line.ns->at(0).dot(offset));
        return tVector(len, Line.ns->at(0));
    };
    auto consolveLineAndPolygon = [](const tVector &offset, const CollisionBox &Line, const CollisionBox &Polygon) -> tVector
    {
        // 不考虑线段两头撞上多边形的情形，本身线段只是作辅助的补充碰撞箱
        //  进行SAT检测
        for (const auto &axis : *(Polygon.ns))
        {
            auto p1 = Line.projectTo(axis);
            auto p2 = Polygon.projectTo(axis);
            p2.addOffset(offset.dot(axis));
            if (p1.high < p2.low || p2.high < p1.low)
            {
                return tVector(0.0, 0.0);
            }
        }
        auto project = Polygon.projectTo(Line.ns->at(0));
        double len = std::min(std::abs(project.low), project.high);
        return tVector(len, Line.ns->at(0));
    };

    auto consolveCircleAndPolygon = [](const tVector &offset, const CollisionBox &Circle, const CollisionBox &Polygon) -> tVector
    {
        // 进行SAT检测
        // 记录最小位移向量
        tVector tAxis = offset.unify(), minV = tAxis;
        auto p1 = Circle.projectTo(tAxis);
        auto p2 = Polygon.projectTo(tAxis);
        p2.addOffset(offset.dot(tAxis));
        if (p1.high < p2.low || p2.high < p1.low)
        {
            return tVector(0.0, 0.0);
        }
        double mMin = (p2.high > p1.high ? p1.high - p2.low : p2.high - p1.low);

        for (const auto &axis : *(Polygon.ns))
        {
            auto p1 = Circle.projectTo(axis);
            auto p2 = Polygon.projectTo(axis);
            p2.addOffset(offset.dot(axis));
            if (p1.high < p2.low || p2.high < p1.low)
            {
                return tVector(0.0, 0.0);
            }
            double temp = (p2.high > p1.high ? p1.high - p2.low : p2.high - p1.low);
            if (mMin > temp)
            {
                mMin = temp;
                minV = axis;
            }
        }
        return minV;
    };
    auto consolveLines = [](const tVector &offset, const CollisionBox &vecA, const CollisionBox &vecB) -> tVector
    {
        // 这里只检测是否相撞，不反回有效的最小分离量
        tVector axis = offset.unify();
        auto p1 = vecA.projectTo(axis);
        auto p2 = vecB.projectTo(axis);
        p2.addOffset(offset.dot(axis));
        if (p1.high < p2.low || p2.high < p1.low)
        {
            return tVector(0.0, 0.0);
        }
        return tVector(1.0, 0.0);
    };
    auto consolveCircles = [](const tVector &offset, const CollisionBox &Ca, const CollisionBox &Cb) -> tVector
    {
        double len = sqrt(offset.sLen());
        if (Ca.radius() + Cb.radius() > len)
        {
            return tVector(0.0, 0.0);
        }
        return tVector(len - Ca.radius() - Cb.radius(), offset);
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

            consolveCircles(offset12, b1, b2);

        if (b2.vectors.size() == 2)

            consolveLineAndCircle(offset12.reverse(), b2, b1);

        else
            consolveCircleAndPolygon(offset12, b1, b2);
    }
    else if (b1.vectors.size() == 2)
    {
        if (b2.vectors.size() == 1)
            consolveLineAndCircle(offset12, b1, b2);
        if (b2.vectors.size() == 2)
            consolveLines(offset12, b1, b2);
        else
            consolveLineAndPolygon(offset12, b1, b2);
    }
    else
    {
        if (b2.vectors.size() == 1)

            return consolveCircleAndPolygon(offset12.reverse(), b2, b1);

        if (b2.vectors.size() == 2)

            return consolveLineAndPolygon(offset12.reverse(), b2, b1);

        // 多边形与多边形
        else
        {

            // 进行SAT检测
            // 记录最小位移向量
            tVector minV(0.0, 0.0);
            double mMin = DBL_MAX;
            for (const auto &axis : *(b1.ns))
            {
                auto p1 = b1.projectTo(axis);
                auto p2 = b2.projectTo(axis);
                p2.addOffset(offset12.dot(axis));
                if (p1.high < p2.low || p2.high < p1.low)
                {
                    return tVector(0.0, 0.0);
                }
                double temp = (p2.high > p1.high ? p1.high - p2.low : p2.high - p1.low);
                if (mMin > temp)
                {
                    mMin = temp;
                    minV = axis;
                }
            }
            for (const auto &axis : *(b2.ns))
            {
                auto p1 = b1.projectTo(axis);
                auto p2 = b2.projectTo(axis);
                p2.addOffset(offset12.dot(axis));
                if (p1.high < p2.low || p2.high < p1.low)
                {
                    return tVector(0.0, 0.0);
                }
                double temp = (p2.high > p1.high ? p1.high - p2.low : p2.high - p1.low);
                if (mMin > temp)
                {
                    mMin = temp;
                    minV = axis;
                }
            }
            return minV;
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
