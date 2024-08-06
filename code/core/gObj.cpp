#include "gObj.h"
template <typename managerT>
gObj& gObj::newObj(managerT &m,gMath::Crdinate& crd){
    
    gObj& t = m.template acquire<gObj>();
    t.setCoordinate(crd);
    return t;
}

template <typename managerT, typename... Args>
gObj& gObj::newObj(managerT &m,gMath::Crdinate& crd ,Args&... args){
    
    gObj& t = m.template acquire<gObj>();
    t.setCoordinate(crd);
    return t;
}
