#ifndef GAME_INSTANCE
#define GAME_INSTANCE
#include "core/gObj.h"


namespace mtype{
    enum gInstanceTypes{
       gInstance,
       StableRectangleObj,
       LiberalRectangleObj
    };
};

#define DECLARE_INSTANCE_TYPE(TypeName)                \
    protected:                                         \
    static constexpr mtype::gInstanceTypes Type = mtype::gInstanceTypes::TypeName; \
    public :                                           \
    virtual constexpr mtype::gInstanceTypes getType() { return Type ; } \
    private: 


class gInstance : virtual public gObj{
    DECLARE_INSTANCE_TYPE(gInstance)

};

namespace mtype{
    template <typename FuncT, typename Obj_pt>
    void applyTemplateMethod( Obj_pt pt, FuncT f , ::gInstance* gInst);

};

#endif
