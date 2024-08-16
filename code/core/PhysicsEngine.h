#ifndef GAME_PHYSICS_ENGINE
#define GAME_PHYSICS_ENGINE

#include "BaseObj.h"
#include "Gmath.h"
#include "Constraint.h"
class PhysicsConstants{
private:
public:
    const double GLOBAL_LINEAR_DAMPING = 0.01;
    const double GLOBAL_ANGULAR_DAMPING = 0.01;
    const gMath::tVector GLOBAL_GRAVITY = gMath::tVector(0, -9.8);
    PhysicsConstants() = default;
    ~PhysicsConstants() = default;
    PhysicsConstants(const double ld, const double ad, const gMath::tVector &g) : GLOBAL_LINEAR_DAMPING(ld), GLOBAL_ANGULAR_DAMPING(ad), GLOBAL_GRAVITY(g) {}
};
#endif