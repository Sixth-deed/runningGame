#ifndef GAME_PHYSICS_ENGINE
#define GAME_PHYSICS_ENGINE

#include "BaseObj.h"
#include "lib/Gmath.h"
#include "Constraint.h"
class PhysicsEngine{
private:
    wCstManager* const cstManager;
public:
    const double GLOBAL_LINEAR_DAMPING = 0.01;
    const double GLOBAL_ANGULAR_DAMPING = 0.01;
    const gMath::tVector GLOBAL_GRAVITY = gMath::tVector(0, -9.8);
    PhysicsEngine() : cstManager(new wCstManager()){ }
    ~PhysicsEngine() = default;
    PhysicsEngine(const double ld, const double ad, const gMath::tVector &g, wCstManager* cstmanager) : 
    cstManager(cstmanager), GLOBAL_LINEAR_DAMPING(ld), GLOBAL_ANGULAR_DAMPING(ad), GLOBAL_GRAVITY(g) {}
    wCstManager* getCstManager() { return cstManager; }
};
#endif