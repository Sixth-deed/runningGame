#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H 

#include <vector>
#include <string>
#include "Gmath.h"
#include "gObjPool.h"
#include "idHandler.h"
#include "QuadTree.h"


#define GRID_NEEDED
#define PRIORITY_LEVEL0
//游戏对象基类
//规定了游戏对象的一些基本属性
class gObj {
    
protected:
    //对象坐标
    gMath::Crdinate crd;
    //对象id
    mID id;
    gMath::Angle angle;
    gObj(gMath::axisV x, gMath::axisV y, gMath::Angle angle_ = 0.0):crd(x,y), angle(angle_){}
    gObj(const gMath::Crdinate& cr,  gMath::Angle angle_ = 0.0):crd(cr), angle(angle_){}
    gObj(): crd(0,0),id(idHandler::mainHandler().getNewID()),angle(0.0) {}
public:
    //用于将关于这个对象的有用的额外信息传往前端
    std::vector<std::string> flags;
    //获得一个对象
    template <typename managerT>
    static gObj& newObj(managerT &m,const gMath::Crdinate& crd, const gMath::Angle &angle_){
        return basicObjInit<gObj>(m , crd, angle_);
    }
    //初始化对象,你应该重载
    static void initObj(gObj* pt, const gMath::Crdinate& crd , const gMath::Angle &angle_){
        pt->crd = crd;
        pt->angle = angle_;
    }
    const gMath::Crdinate& getCrd() const {
        return crd;
    }
    mID getID() const { return id; }
    void setCoordinate(gMath::Crdinate crd_){
        crd=crd_;
    }
    gMath::Angle getAngle() const {
        return angle;
    }
    gMath::Angle setAngle(const gMath::Angle& angle_) {
        angle = angle_;

    }
    static const bool movable = false;
    static const bool rotatable = false;
    static const bool isEntity = false;
};
template <typename managerT, typename gObjType>
inline gObjType& basicObjInit(managerT& m,const gMath::Crdinate& crd = gMath::Crdinate(0, 0 ), const gMath::Angle& angle_ = 0.0){
    gObjType& gobj = m.template acquire<gObjType>();
    gobj.setCoordinate(crd);
    gobj.angle = angle_;
    return gobj;
}
#endif 
