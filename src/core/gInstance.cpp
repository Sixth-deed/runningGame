#include "GameObjects/allObjects.h"

namespace mtype{
    /*
    //废案，无用
    template <typename FuncT, typename Obj_pt>
    void applyTemplateMethod( Obj_pt pt, FuncT f , ::gInstance* gInst)
{
        switch (gInst->getType()){
            case gInstanceTypes::gInstance:
                pt->f(gInst);
                break;
            case gInstanceTypes::StableRectangleObj:
                pt->f(reinterpret_cast<::StableRectangleObj*>(gInst));
                break;
            case gInstanceTypes::LiberalRectangleObj : 
                pt->f(reinterpret_cast<::LiberalRectangleObj*>(gInst));
                break;
        
        }
    }*/

};
