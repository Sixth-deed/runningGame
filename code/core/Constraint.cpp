#include "Constraint.h"
#include "ExtendedPhysicsObj.h"
#include "lib/mOptional.h"
#include "Collision.h"
#include <cmath>
void ContactConstraint::update()
{
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
        case 1:
            MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
            if (!pt1->isSleep() && normal.dot(pt1->getVelocity()) < 0.0)
                cstState = ConstraintState::Seperating;
        case 2:
            MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
            if (!pt2->isSleep() && normal.dot(pt2->getVelocity()) > 0.0)
                cstState = ConstraintState::Seperating;
        case 3:
            MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
            MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
            if (!(pt1->isSleep() && pt2->isSleep()) && (normal.dot(pt1->getVelocity() - pt2->getVelocity()) < 0.0))
                cstState = ConstraintState::Seperating;
        }
        break;
    case ConstraintState::Seperating:
        if (!isReallyIntersects(*ett1, *ett2))
        {
            release();
        }
        else
        {
            switch (moveableFlag)
            {
            case 0:
                // Error
            case 1:
                MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
                if (!pt1->isSleep() && normal.dot(pt1->getVelocity()) > 0.0)
                    cstState = ConstraintState::OnGoing;
            case 2:
                MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
                if (!pt2->isSleep() && normal.dot(pt2->getVelocity()) < 0.0)
                    cstState = ConstraintState::OnGoing;
            case 3:
                MoveObj *pt1 = reinterpret_cast<MoveObj *>(ett1);
                MoveObj *pt2 = reinterpret_cast<MoveObj *>(ett2);
                if (!(pt1->isSleep() && pt2->isSleep()) && (normal.dot(pt1->getVelocity() - pt2->getVelocity()) > 0.0))
                    cstState = ConstraintState::OnGoing;
            }
        }
        break;
    }
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
    if (lambda_t > 0.0)
    {
        const tVector &impulse_t = tangent * lambda_t;
        pt1->applyImpulse_v(impulse_t, toCp1);
        pt2->applyImpulse_v(impulse_t.reverse(), toCp2);
    }
    // 求解弹性冲量
    double lambda_n = -normal_inv_effectiveMass * normalDv * restitution;
    const double n_otI = oldNormalImpulse;
    oldNormalImpulse = std::max(0.0, oldNormalImpulse + lambda_n);
    lambda_n = oldNormalImpulse - n_otI;
    if (lambda_n > 0.0)
    {
        const tVector &impulse_n = normal * lambda_n;
        pt1->applyImpulse_v(impulse_n, toCp1);
        pt2->applyImpulse_v(impulse_n.reverse(), toCp2);
    }
}

void PhysicsContactConstraint::solve_p()
{
    const gMath::tVector delta = (ett1->getCrd() + toCp1) - (ett2->getCrd() + toCp2);
    if (delta.dot(normal) > 0)
    {
        return;
    }
    const double penetration = -delta.dot(normal);
    const double bias = std::max(0.0, maxPenetraintion - penetration * posBiasFactor);
    const double lambda = -normal_inv_effectiveMass * bias;
    if (lambda > 0.0)
    {
        const gMath::tVector impulse = normal * lambda;
        pt1->moveFix(impulse.reverse() * pt1->getInverseMass());
        pt1->rotateFix(-(toCp1.cross(impulse) * pt1->getInverseInertia()));

        pt2->moveFix(impulse * pt2->getInverseMass());
        pt2->rotateFix(toCp2.cross(impulse) * pt2->getInverseInertia());
    }
}

template <typename... cstTypes>
inline void ConstraintManager<cstTypes...>::checkAndUpdate(EntityObj *ett1p, EntityObj *ett2p)
{
    switch (whatCollide(*ett1p, *ett2p))
    {
    case CollisionType::NoCollision:
        break;
    case CollisionType::NormalCollision:
        if (isOuterIntersects(*ett1p, *ett2p) && cstGraph->containsEdge(ett1p, ett2p) && !(ett1p->isSleep() && ett2p->isSleep()))
        {
            mOptional<clsn::CollisionLocal> cl = isReallyIntersects(*ett1p, *ett2p);
            if (cl)
            {
                NormalContactConstraint::newConstraint(*this, ett1p, ett2p, std::move(cl->t1cp), std::move(cl->t2cp), std::move(cl->normal));
            }
        }
    case CollisionType::PhysicsCollision:
        if (isOuterIntersects(*ett1p, *ett2p) && cstGraph->containsEdge(ett1p, ett2p) && !(ett1p->isSleep() && ett2p->isSleep()))
        {
            mOptional<clsn::CollisionLocal> cl = isReallyIntersects(*ett1p, *ett2p);
            if (cl)
            {
                PhysicsContactConstraint::newConstraint(*this, ett1p, ett2p, std::move(cl->t1cp), std::move(cl->t2cp), std::move(cl->normal));
            }
        }
    }
}

void SolveChain::updateAll()
{
    Constraint *pt = head;
    while (pt)
    {
        pt->update();
        pt = pt->next;
    }
}

template <typename... cstTypes>
void ConstraintManager<cstTypes...>::integrateAll()
{
    while (!toIntegrate.empty())
    {
        toIntegrate.top()->Integrate(1);
        toIntegrate.pop();
    }
}

template <typename... cstTypes>
void ConstraintManager<cstTypes...>::clear()
{
    cstAllocManager.clear();
    for (auto solver_p : chains)
    {
        delete solver_p;
    }
    toIntegrate.clear();
}

template <typename... cstTypes>
void ConstraintManager<cstTypes...>::solveAll()
{
    for (auto solver_p : chains)
    {
        solver_p->solveAll();
    }
}

template <typename... cstTypes>
void ConstraintManager<cstTypes...>::updateAll()
{
    for (auto solver_p : chains)
    {
        solver_p->updateAll();
    }
}

SolveChain::~SolveChain()
{
    Constraint *pt = head;
    while (pt)
    {
        Constraint *next = pt->next;
        delete pt;
        pt = next;
    }
}

void NormalSolver::solveAll()
{
    Constraint *pt = head;
    while (pt)
    {
        pt->solve();
        pt = pt->getNext();
    }
}

void VelAndPosSolver::solveAll()
{
    {
        Constraint *pt = head;
        while (pt)
        {
            pt->solve();
            pt = pt->getNext();
        }
    }
    for (int i = 0; i < vIterations; i++)
    {
        PhysicsContactConstraint *pt = reinterpret_cast<PhysicsContactConstraint *>(head);
        while (pt)
        {
            pt->solve_v();
            pt = pt->getNext();
        }
    }
    cstManager->integrateAll();

    for (int i = 0; i < pIterations; i++)
    {
        PhysicsContactConstraint *pt = reinterpret_cast<PhysicsContactConstraint *>(head);
        while (pt)
        {
            pt->solve_p();
            pt = pt->getNext();
        }
    }
    // 可能要check，仍出现过大的重合冻结画面
}

bool ContactConstraint::seperated()
{
    const gMath::tVector delta = (ett1->getCrd() + toCp1) - (ett2->getCrd() + toCp2);
    return delta.dot(normal) > 0;
}