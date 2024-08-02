#include "gObj.h"

gObj& gObj::newObj(Crdinate& crd){
    
    gObj& t = *MainObjectManager.acquire<gObj>();
    t.setCoordinate(crd);
    return t;
}
