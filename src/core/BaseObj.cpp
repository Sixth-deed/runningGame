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

template<>
bool Grid<EntityObj>::Inside(const gObj *pt) const{
    const EntityObj* const pett = reinterpret_cast<const EntityObj*>(pt);
    return rect.Inside( pett->getCrd() ,pett->get_c_CollisionBox().getShape_c());
}

PhysicsEngine* PhysicsObj::mainPhysicsEngine = nullptr;
