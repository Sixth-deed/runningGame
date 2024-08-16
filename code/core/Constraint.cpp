#include "Constraint.h"
#include "ExtendedPhysicsObj.h"
#include "lib/mOptional.h"
#include "Collision.h"
#include <cmath>
void ContactConstraint::update()
{
    // 逻辑有问题，择日重写
    switch (cstState)
    {
    case ConstraintState::NewlyAdded:
        cstState = ConstraintState::OnGoing;
        break;
    case ConstraintState::OnGoing:
        switch (moveableFlag)
        {
        case 0:
            // Error
            break;
        case 1:
            MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
            if (!pt1->isMoved())
                cstState = ConstraintState::Sleep0;
            if (normal.dot(pt1->getVelocity()) < 0.0)
                cstState = ConstraintState::Seperating;
            break;
        case 2:
            MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
            if (!pt2->isMoved())
                cstState = ConstraintState::Sleep0;
            if (normal.dot(pt2->getVelocity()) < 0.0)
                cstState = ConstraintState::Seperating;
            break;
        case 3:
            MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
            MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
            if (pt1->isMoved() && pt2->isMoved() && (normal.dot(pt1->getVelocity() - pt2->getVelocity()) < 0.0))
                cstState = ConstraintState::Seperating;
            else if (pt1->isMoved() && (pt1->getVelocity().dot(normal) < 0.0))
                cstState = ConstraintState::Seperating;
            else if (pt2->isMoved() && (pt2->getVelocity().dot(normal) < 0.0))
                cstState = ConstraintState::Seperating;
            else
                cstState = ConstraintState::Sleep0;
            break;
        }
        break;
    case ConstraintState::Seperating:
        if (!broadPhaseTest())
        {
            cstState = ConstraintState::toBeRemoved;
        }
        else
        {
            switch (moveableFlag)
            {
            case 0:
                // Error
                break;
            case 1:
                MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
                if (!pt1->isMoved())
                    cstState = ConstraintState::Sleep0;
                if (normal.dot(pt1->getVelocity()) > 0.0)
                    cstState = ConstraintState::OnGoing;
                break;
            case 2:
                MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
                if (!pt2->isMoved())
                    cstState = ConstraintState::Sleep0;
                if (normal.dot(pt2->getVelocity()) > 0.0)
                    cstState = ConstraintState::OnGoing;
                break;
            case 3:
                MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
                MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
                if (pt1->isMoved() && pt2->isMoved() && (normal.dot(pt1->getVelocity() - pt2->getVelocity()) > 0.0))
                    cstState = ConstraintState::OnGoing;
                else if (pt1->isMoved() && (pt1->getVelocity().dot(normal) > 0.0))
                    cstState = ConstraintState::OnGoing;
                else if (pt2->isMoved() && (pt2->getVelocity().dot(normal) > 0.0))
                    cstState = ConstraintState::OnGoing;
                else
                    cstState = ConstraintState::Sleep0;
                break;
            }
        }
        break;
    case ConstraintState::Sleep0:
        switch (moveableFlag)
        {
        case 0:
            // Error
        case 1:
            MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
            if (pt1->isMoved())
                cstState = ConstraintState::OnGoing;
            else
            {
                cstState = ConstraintState::Sleeping;
                pt1->setSleep();
            }
            break;
        case 2:
            MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
            if (pt2->isMoved())
                cstState = ConstraintState::OnGoing;
            else
            {
                cstState = ConstraintState::Sleeping;
                pt2->setSleep();
            }
            break;
        case 3:

            MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
            MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
            if (pt1->isMoved() || pt2->isMoved())
                cstState = ConstraintState::OnGoing;
            else
            {
                cstState = ConstraintState::Sleeping;
                pt1->setSleep();
                pt2->setSleep();
            }
            break;
        default:
            // Error
            break;
        }

        break;
    case ConstraintState::Sleeping:
        switch (moveableFlag)
        {
        case 0:
            // Error
        case 1:
            MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
            if (!pt1->isSleep())
                cstState = ConstraintState::OnGoing;
            break;
        case 2:
            MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
            if (!pt2->isSleep())
                cstState = ConstraintState::OnGoing;
            break;
        case 3:

            MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
            MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
            if (!pt2->isSleep() || !pt1->isSleep())
                cstState = ConstraintState::OnGoing;
            break;
        default:
            // Error
            break;
        }
        break;
    default:
        break;
    }
}

bool NormalContactConstraint::broadPhaseTest()
{
    return isOuterIntersects(*ett1, *ett2);
}

void PhysicsContactConstraint::init()
{
    
    tangent = normal.normal_and_unify();
    const double ivm1 = pt1->getInverseMass();
    const double ivm2 = pt2->getInverseMass();

    const double ivI1 = pt1->rotatable ? reinterpret_cast<RotatePhysicsObj *>(pt1)->getInverseInertia() : 0.0;
    const double ivI2 = pt2->rotatable ? reinterpret_cast<RotatePhysicsObj *>(pt2)->getInverseInertia() : 0.0;
    const double rn1 = toCp1.dot(tangent);
    const double rn2 = toCp2.dot(tangent);
    const double Knormal = ivm1 + ivm2 + rn1 * rn1 * ivI1 + rn2 * rn2 * ivI2;
    normal_inv_effectiveMass = Knormal == 0 ? 1.0 / Knormal : 0.0;

    const double rt1 = toCp1.dot(normal);
    const double rt2 = toCp2.dot(normal);
    const double Ktangent = ivm1 + ivm2 + rt1 * rt1 * ivI1 + rt2 * rt2 * ivI2;
    tangent_inv_effectiveMass = Ktangent == 0 ? 1.0 / Ktangent : 0.0;

    const gMath::tVector &wv1 = pt1->rotatable ? toCp1.cross(reinterpret_cast<RotatePhysicsObj *>(pt1)->getAngleVelocity()) : gMath::tVector(0.0, 0.0);
    const gMath::tVector &wv2 = pt2->rotatable ? toCp2.cross(reinterpret_cast<RotatePhysicsObj *>(pt2)->getAngleVelocity()) : gMath::tVector(0.0, 0.0);
    const gMath::tVector &v1 = pt1->movable ? reinterpret_cast<MoveObj *>(pt1)->getVelocity() : gMath::tVector(0.0, 0.0);
    const gMath::tVector &v2 = pt2->movable ? reinterpret_cast<MoveObj *>(pt2)->getVelocity() : gMath::tVector(0.0, 0.0);
    dv = wv1 + v1 - wv2 - v2;
}

void PhysicsContactConstraint::solve_v()
{
    using namespace gMath;
    ett1->CollisionAct(*ett2);
    ett2->CollisionAct(*ett1);

    // 求解摩擦冲量
    const double tangentDv = tangent.dot(dv), normalDv = normal.dot(dv);
    const double maxtangentimpulse = friction * oldNormalImpulse;
    double lambda_t = -tangent_inv_effectiveMass * tangentDv;
    const double t_otI = oldTangentImpulse;
    oldTangentImpulse = gMath::clamp(oldTangentImpulse + lambda_t, -maxtangentimpulse, maxtangentimpulse);
    lambda_t = oldTangentImpulse - t_otI;

    const tVector &impulse_t = tangent * lambda_t;
    pt1->applyImpulse_v(impulse_t, toCp1);
    pt2->applyImpulse_v(impulse_t.reverse(), toCp2);

    // 求解弹性冲量
    double lambda_n = -normal_inv_effectiveMass * normalDv * restitution;
    const double n_otI = oldNormalImpulse;
    oldNormalImpulse = std::max(0.0, oldNormalImpulse + lambda_n);
    lambda_n = oldNormalImpulse - n_otI;
    const tVector &impulse_n = normal * lambda_n;
    pt1->applyImpulse_v(impulse_n, toCp1);
    pt2->applyImpulse_v(impulse_n.reverse(), toCp2);
}

void PhysicsContactConstraint::solve_p()
{
    // your code here
}

template <typename... cstTypes>
inline void ConstraintManager<cstTypes...>::checkAndUpdate(EntityObj *ett1p, EntityObj *ett2p)
{
    switch (whatCollide(*ett1p, *ett2p))
    {
    case CollisionType::NoCollision:
        break;
    case CollisionType::NormalCollision:
        if (isOuterIntersects(*ett1p, *ett2p) && cstGraph->containsEdge(ett1p , ett2p) && !( ett1p -> isSleep()&& ett2p -> isSleep()))
        {
            mOptional<clsn::CollisionLocal> cl = isReallyIntersects(*ett1p, *ett2p);
            if (cl)
            {                
                NormalContactConstraint::newConstraint(*this, ett1p, ett2p, std::move(cl->t1cp), std::move(cl->t2cp), std::move(cl->normal));
            }
        }
    case CollisionType::PhysicsCollision:
        if (isOuterIntersects(*ett1p, *ett2p) && cstGraph->containsEdge(ett1p , ett2p) && !( ett1p -> isSleep()&& ett2p -> isSleep()))
        {
            mOptional<clsn::CollisionLocal> cl = isReallyIntersects(*ett1p, *ett2p);
            if (cl)
            {                
                PhysicsContactConstraint::newConstraint(*this, ett1p, ett2p, std::move(cl->t1cp), std::move(cl->t2cp), std::move(cl->normal));
            }
        }
    }
}