#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H 

#include <vector>
#include <string>
#include "Gmath.h"
#include "ObjPool.h"
#include "idHandler.h"
#include "QuadTree.h"


#define GRID_NEEDED
#define PRIORITY_LEVEL0
//游戏对象基类
//规定了游戏对象的一些基本属性
class gObj {
    
protected:

    //友元类
    template <typename U>
    friend class ObjPool;

    //对象坐标
    gMath::Crdinate crd;
    //对象id
    mID id;
    gMath::Angle angle;
    gObj(gMath::axisV x, gMath::axisV y, gMath::Angle angle_ = 0.0):crd(x,y), angle(angle_){}
    gObj(const gMath::Crdinate& cr,  gMath::Angle angle_ = 0.0):crd(cr), angle(angle_){}
    gObj(): crd(0,0),id(idHandler::mainHandler().getNewID()),angle(0.0) {}
    //用来记录是否被遍历与更新，除了ActObj，其他类固定为false
    bool isSleeping =false;
public:
    
    //用于将关于这个对象的有用的额外信息传往前端
    std::vector<std::string> flags;
    //获得一个对象
    template <typename managerT>
    static gObj& newObj(managerT &m,const gMath::Crdinate& crd, const gMath::Angle &angle_){
        return basicObjInit<gObj>(m , crd, angle_);
    }
    //初始化对象,子类应该重载
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
    bool isSleep(){return isSleeping;}
    void setSleep(){isSleeping = true;}



    //此处的SendPackType类型需要实现，Container选择合适的容器即可
    SendPackType send() {
        //to be finished
        //应该完成当前对象坐标，对象id，对象角度，对象类型以及flags的打包
        
        return SendPackType(...);      

        //当前能想到的需要做的优化：

        //1.使用右值传递SendPackType, 避免拷贝
        //2.这里获取对象类型尽量不用c++运行时动态类型，保证能够编译时确定
        
        
    }

    //另一种可选的发送方式
    void send(SendPackContainer& container){
        container.push((SendPackType)...); 
    }
};
template <typename managerT, typename gObjType>
inline gObjType& basicObjInit(managerT& m,const gMath::Crdinate& crd = gMath::Crdinate(0, 0 ), const gMath::Angle& angle_ = 0.0){
    gObjType& gobj = m.template acquire<gObjType>();
    gobj.setCoordinate(crd);
    gobj.angle = angle_;
    return gobj;
}
#endif 
