#include "gObj.h"

gObj& gObj::newObj(gMath::Crdinate& crd){
    
    gObj& t = *MainObjectManager.acquire<gObj>();
    t.setCoordinate(crd);
    return t;
}
