#include "BaseObj.h"
#include "lib/QuadTree.h"
#include "Constraint.h"
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
    
}

template<>
bool Grid<EntityObj>::Inside(const EntityObj *pett) const{
    return rect.Inside( pett->getCrd() ,pett->get_c_CollisionBox().getShape_c());
}