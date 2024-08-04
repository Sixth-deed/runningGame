#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H 

#include <vector>
#include <string>
#include "Gmath.h"
#include "gObjPool.h"
//ID类型
using mID = unsigned int;

//游戏对象基类
//规定了游戏对象的一些基本属性
class gObj{
    
protected:
    //对象坐标
    gMath::Crdinate crd;
    //对象id
    mID id;
    gObj(): crd(0,0),id(idHandler::mainHandler().getNewID()) {}
public:
    //用于将关于这个对象的有用的额外信息传往前端
    std::vector<std::string> flags;
    //获得一个对象
    static gObj& newObj(gMath::Crdinate& );
    const gMath::Crdinate& getCrd() const {
        return crd;
    }
    mID getID() const { return id; }
    void setCoordinate(gMath::Crdinate crd_){
        crd=crd_;
    }
};
#endif 
