#include "gObj.h"
#include "Gmath.h"
template <typename managerT>
gObj& gObj::newObj(managerT &m,const gMath::Crdinate& crd, const gMath::Angle& angle_){
    return basicObjInit<gObj>(m , crd, angle_);
}
