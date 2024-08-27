#ifndef RECTANGLE_OBJECT
#define RECTANGLE_OBJECT

#include "core/ExtendedPhysicsObj.h"
#include <ctime>
#include <vector>

class StableRectangleObj : public StablePhysicsObj
{
    DECLARE_INSTANCE_TYPE(StableRectangleObj)

public:
    StableRectangleObj() : StablePhysicsObj() {}
    static void initObj(StablePhysicsObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &&cl,
                        double mass_ = 0.0, double friction_C_ = 0.0, double restitution_C_ = 1.0, bool graviityAffected_ = false, bool dragAffected_ = false)
    {
        StablePhysicsObj::initObj(pt, crd, angle_, std::move(cl), mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_);
    }
    template <typename managerT>
    static StableRectangleObj &newObj(managerT &m, const gMath::Crdinate crd_, const gMath::Angle &angle_, const gMath::tVector &toLeftTop, double friction_C_ = 0.0, double restitution_C_ = 1.0)
    {
        StableRectangleObj &to = *(m.template acquire<StableRectangleObj>());
        const double x = toLeftTop.x;
        const double y = toLeftTop.y;
        initObj(&to, crd_, angle_, clsn::CollisionBox(std::vector<gMath::tVector>{toLeftTop, gMath::tVector(-x, y), gMath::tVector(-x, -y), gMath::tVector(x, -y)}), 0.0, friction_C_, restitution_C_);
        return to;
    }
    template <typename managerT>
    void release(managerT &m)
    {
        m.template release<StableRectangleObj>(this);
    }
    bool CollisionAct(EntityObj &r) override { return true; }
};
class LiberalRectangleObj : public LiberalPhysicsObj
{
    DECLARE_INSTANCE_TYPE(LiberalRectangleObj)
public:
    static void initObj(LiberalPhysicsObj *t, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &&cl,
                        double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_,
                        const double angleV = 0.0, const double angleA = 0.0,
                        const gMath::tVector &initialVelocity = gMath::tVector(0.0, 0.0), const gMath::tVector &initialAccelr = gMath::tVector(0.0, 0.0))
    {
        LiberalPhysicsObj::initObj(t, crd, angle_, std::move(cl), mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, angleV, angleA, initialVelocity, initialAccelr);
    }
    template <typename managerT>
    static LiberalRectangleObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, const gMath::tVector &toLeftTop,
                                       double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_,
                                       const double angleV = 0.0, const double angleA = 0.0,
                                       const gMath::tVector &initialVelocity = gMath::tVector(0.0, 0.0), const gMath::tVector &initialAccelr = gMath::tVector(0.0, 0.0))
    {
        LiberalRectangleObj &to = *(m.template acquire<LiberalRectangleObj>());
        const double x = toLeftTop.x;
        const double y = toLeftTop.y;
        initObj(&to, crd, angle_, clsn::CollisionBox(std::vector<gMath::tVector>{toLeftTop, gMath::tVector(-x, y), gMath::tVector(-x, -y), gMath::tVector(x, -y)}, true, angle_), mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, angleV, angleA, initialVelocity, initialAccelr);
        return to;
    }
    template <typename managerT>
    void release(managerT &m)
    {
        m.template release<LiberalRectangleObj>(this);
    }
    bool CollisionAct(EntityObj &r) override { return true; }
};

#endif
