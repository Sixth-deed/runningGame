#include "gObj.h"
template <typename managerT>
gObj& gObj::newObj(gMath::Crdinate& crd, managerT& m ){
    
    gObj& t = m.acquire<gObj>();
    t.setCoordinate(crd);
    return t;
}
