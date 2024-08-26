#include "core/BaseObj.h"
#include "core/QuadTree.h"
#include "core/Constraint.h"
#include <memory>

idHandler* gObj::mIdHandler = nullptr;


std::vector<ActiveRectangle *> ActiveRectangle::rects;
template<>
template <typename... Args>
bool Grid<EntityObj>::recursionForInnerGrid(EntityObj *pObj, bool (Grid<EntityObj>::*method)(Args...),Args... args)
{   
    bool flag = false;
    if (rt -> Inside(pObj))
        flag |= (rt->*method)(args...);
    if (lt -> Inside(pObj))
        flag |= (lt->*method)(args...);
    if (rb -> Inside(pObj))
        flag |= (rb->*method)(args...);
    if (lb -> Inside(pObj))
        flag |= (lb->*method)(args...);
    return flag;    
}



PhysicsEngine* PhysicsObj::mainPhysicsEngine = nullptr;
