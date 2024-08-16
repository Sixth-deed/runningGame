#include "BaseObj.h"
#include "Gmath.h"
namespace clsn{
    using namespace gMath;
    class Contact{
        private:
        //接触点
        Crdinate cp;
        //对象1
        EntityObj* ett1;
        //对象2
        EntityObj* ett2;
        //接触法线, 规定为由对象1指向对象2
        tVector normal;
        //接触切线
        tVector tangant;
        //接触类型
        CollisionType cty;
        public :
        Contact (const Crdinate& crd_, EntityObj* ett1_, EntityObj* ett2_, const  tVector& normal_):
        cp(crd_), ett1(ett1_), ett2(ett2_), normal(normal_), tangant(normal_.normal_and_unify()), cty(whatCollide(*ett1_, *ett2_)){}
    
         
    };

    
}
