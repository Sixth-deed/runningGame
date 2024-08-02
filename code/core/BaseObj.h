#ifndef BASIC_GAME_OBJECT
#define BASIC_GAME_OBJECT
#include "gObj.h"


//实体对象，拥有碰撞体积，可以参与碰撞体积检测
//发生有效碰撞后执行CollisionAct()方法
class EntityObj;

//拥有Act()方法的对象
class ActObj;

//可移动的对象
//有速度和加速度属性
class MoveObj;

//有物理属性的对象
//无速度和加速度属性
class PhysicsObj;

//有附加行动的对象
//即在预定义（编译时）就设定好的常规行动Act()之外有可能的运行时添加行动的对象，拥有addExtraAct和removeExtraAct方法，传入函数指针
//已经是ActObj的子类
class ExtraActObj;

//有监听器的对象
//即有可能需要在运行时判定某个条件以执行代码的对象,有addLiscenerOn()和removeLiscenerOn方法
//与ExtraActObj的主要不同是自带条件判断，异步执行
class LiscenerObj;

#include "Collision.h"
class EntityObj : virtual public gObj{
protected:
    //碰撞箱
    clsn::CollisionBox clsnBox;
public:
    virtual bool CollisionAct(EntityObj& another_obj) = 0;
};
#endif
