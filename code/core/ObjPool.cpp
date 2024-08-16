#include "ObjPool.h"
template <typename gObjType>
gObjType& ObjPool<gObjType>::acquire(){
    if (pool.empty()){
        return static_cast<gObjType&>(*(new gObjType()));
    }
    else{
        gObjType* pt = pool.back();
        pool.pop_back();
        return *pt;
    }
}
