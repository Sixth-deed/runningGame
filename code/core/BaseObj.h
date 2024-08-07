#ifndef BASIC_GAME_OBJECT
#define BASIC_GAME_OBJECT
#include "gObj.h"


//实体对象，拥有碰撞体积，可以参与碰撞体积检测
//发生有效碰撞后执行CollisionAct()方法
class EntityObj;

//拥有Act,addNewAction和removeAction方法的对象
class ActObj;

//可移动的对象
//有速度和加速度属性
class MoveObj;

//有物理属性的对象
//无速度和加速度属性
class PhysicsObj;


//有监听器的对象
//即有可能需要在运行时判定某个条件以执行代码的对象,有addLiscenerOn()和removeLiscenerOn方法
//与ExtraActObj的主要不同是自带条件判断，异步执行
class LiscenerObj;

//保持片区活跃的对象
//保持对对象activeRectangle范围内所有对象的更新
class ActivateObj;

#include "Collision.h"
class EntityObj : virtual public gObj{
protected:
    //碰撞箱
    clsn::CollisionBox clsnBox;
public:
    const clsn::CollisionBox& getCollisionBox() const {return clsnBox;} 
    virtual bool CollisionAct(EntityObj& another_obj) = 0;
};
//检测两个实体对象有没有发生碰撞
inline gMath::tVector isIntersects(const EntityObj& b1, const EntityObj& b2){
        return clsn::isIntersects(b1.getCrd(), b1.getCollisionBox(), b2.getCrd(), b2.getCollisionBox());
    }



class ActObj : virtual public gObj{

};



class MoveObj : virtual public ActObj{

};


class ActivateObj :virtual public gObj{
private:
    ActiveRectangle activeRectangle;
public:
    //(x,y) 中心坐标
    //toLeftTop 由中心指向方形格左上角顶点的向量
    ActivateObj(gMath::axisV x, gMath::axisV y, gMath::tVector toLeftTop):
    gObj(x,y),
    activeRectangle(x + toLeftTop.x, x - toLeftTop.x, y + toLeftTop.y, y - toLeftTop.y)
    {} 
};

#endif