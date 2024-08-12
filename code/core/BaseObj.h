#ifndef BASIC_GAME_OBJECT
#define BASIC_GAME_OBJECT
#include "Gmath.h"
#include "gObj.h"
#include "idHandler.h"
#include <csignal>
#include <cstdlib>
#include <memory>
#include <new>
#include <unordered_map>

#define GRID_NEEDED
#define PRIORITY_LEVEL5
//实体对象，拥有碰撞体积，可以参与碰撞体积检测
//发生有效碰撞后执行CollisionAct()方法
class EntityObj;

#define GRID_NEEDED
#define PRIORITY_LEVEL1
//拥有act方法的对象
//重载act时不要忘了调用基类的act
class ActObj;



//拥有act,addNewAction和removeAction方法的对象
//与ActObj不同的是，它可以在运行时动态地添加行为
class DynamicActObj;

//可移动的对象
//有速度和加速度属性
class MoveObj;


//可旋转的对象
//有中心点和角速度属性
class RotateObj;


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
    clsn::CollisionBox* clsnBox;
    EntityObj () : gObj(), clsnBox(nullptr) {}
public:
    const clsn::CollisionBox& getCollisionBox() const {return *clsnBox;} 
    virtual bool CollisionAct(EntityObj& another_obj) = 0;

    template <typename managerT>
    static EntityObj& newObj(managerT & m, const gMath::Crdinate& crd = gMath::Crdinate(0 , 0), const gMath::Angle& angle_ = 0.0){
       return basicObjInit<EntityObj>(m , crd, angle_);
    }
    template <typename managerT>
    static EntityObj& newObj(managerT & m, const gMath::Crdinate& crd,const gMath::Angle& angle_, clsn::CollisionBox& cl){
       EntityObj& t =  basicObjInit<EntityObj>(m , crd, angle_);
       t.clsnBox = &cl;
       return t;}
};
//检测两个实体对象有没有发生碰撞
inline gMath::tVector isIntersects(const EntityObj& b1, const EntityObj& b2){
        return clsn::isIntersects(b1.getCrd(), b1.getCollisionBox(), b2.getCrd(), b2.getCollisionBox());
    }

class ActObj : virtual public gObj{
public :
    //写入当前类的行为
    virtual void act() = 0;
};

class DynamicActObj : virtual public ActObj{
protected:
    std::unordered_map<std::string,void(*)(DynamicActObj*)>* actions;
    DynamicActObj(): gObj(),actions(new std::unordered_map<std::string,void(*)(DynamicActObj*)>(1)){}
    DynamicActObj(gMath::axisV x, gMath::axisV y): gObj(x,y), actions(new std::unordered_map<std::string,void(*)(DynamicActObj*)>(1)){}
public:
    void act() override {
        for (auto f : *actions){
            f.second(this);
        }
    }
    void addNewAction(const std::string& funcName,void (*f)(DynamicActObj*)){
        (*actions)[funcName]=f;
    }
    void removeAction(const std::string& funcName){
        (*actions).erase(funcName);
    }
    template <typename managerT>
    static DynamicActObj& newObj(managerT &m,const gMath::Crdinate& crd, gMath::Angle angle_ = 0.0){
        return basicObjInit<DynamicActObj>(m, crd, angle_);
    }
    
    template <typename managerT, typename IteratorType>
    static DynamicActObj& newObj(managerT &m,const gMath::Crdinate& crd, gMath::Angle angle_ ,IteratorType &begin, IteratorType& end){
        DynamicActObj& t = basicObjInit<DynamicActObj>(m, crd, angle_);
        delete t.actions;
        t.actions = new std::unordered_map(begin, end);
        return t;
    }
    template <typename managerT>
    static DynamicActObj& newObj(managerT &m,const gMath::Crdinate& crd, gMath::Angle angle_,std::unordered_map<std::string, void(*)(DynamicActObj*)>& acts){
        DynamicActObj& t = basicObjInit<DynamicActObj>(m, crd, angle_);
        delete t.actions;
        t.actions = &acts;
        return t;
    }
};


class MoveObj : virtual public ActObj{
    using tVector = gMath::tVector;
private:
    tVector velocity;
    
    tVector acceleration;
    
    MoveObj(): ActObj(), velocity(0.0,0.0) , acceleration(0.0,0.0){}
    
    //为0时直接删除对象
    //为1时保留对象速度，直到对象再次在活跃范围中才更新对象位置
    //为2时对象速度和加速度清零，再次在活跃范围中被加载
    //为3时调用自定义的处理方法
    static const int onOffMethodSignal = 0;
    
    //自定义的处理离开视野的方法
    //return 0表示删除对象，return 1会保留对象
    int s_onOffTackler(){return 0;}
public:
    template <typename managerT>
    static MoveObj& newObj(managerT &m,const gMath::Crdinate& crd, gMath::Angle angle_ = 0.0){
        return basicObjInit<MoveObj>(m, crd, angle_);
    }
    int onOffTackler(){
        switch(onOffMethodSignal){
            case 0:
                return 0;
            case 1:
                return 1;
            case 2:
                setVelocity(tVector(0.0,0.0));
                setAcceleration(tVector(0.0,0.0));
                return onOffMethodSignal;
            case 3:
                return s_onOffTackler();
        }
       
    }   
    template <typename managerT, typename IteratorType>
    static MoveObj& newObj(managerT &m,const gMath::Crdinate& crd, gMath::Angle angle_ , const tVector  initialVelocity = tVector(0.0 , 0.0), const tVector& initialAccelr = (0.0, 0.0)){
        MoveObj& t = basicObjInit<MoveObj>(m, crd, angle_); 
        t.velocity = initialVelocity;
        t.acceleration = initialAccelr;
        return t;
    }
    const tVector& getVelocity() const {
        return velocity;
    }
    void setVelocity(const tVector &v) {
        velocity = v;
    }
    const tVector& getAcceleration() const {
        return acceleration;
    }
    void setAcceleration(const tVector &a){
        acceleration = a;
    }
    void act() override{
        velocity += acceleration;
        crd += velocity;
    }
    static const bool movable = true;
};


class RotateObj : virtual public ActObj{
protected:
    //角速度
    gMath::Angle angleVelocity;
    //旋转中心默认为坐标点
    
    RotateObj():ActObj(), angleVelocity(0){}
public:
    void act() override {
        angle += angleVelocity;
    }
    template <typename managerT>
    static RotateObj& newObj(managerT &m,const gMath::Crdinate& crd, gMath::Angle angle_ = 0.0){
        return basicObjInit<RotateObj>(m, crd, angle_);
    }
    template <typename managerT, typename IteratorType>
    static RotateObj& newObj(managerT &m,const gMath::Crdinate& crd, gMath::Angle angle_ , const gMath::Angle& angleV = 0.0){
        RotateObj& t = basicObjInit<MoveObj>(m, crd, angle_); 
        t.angleVelocity = angleV;
        return t;
    }
    static const bool rotatable = true;
};

class ActivateObj :virtual public gObj{
private:
    ActiveRectangle* activeRectangle;
    //(x,y) 中心坐标
    //toLeftTop 由中心指向方形格左上角顶点的向量
    ActivateObj(gMath::axisV x, gMath::axisV y, gMath::tVector toLeftTop):
    gObj(x,y),
    activeRectangle(new ActiveRectangle(x + toLeftTop.x, x - toLeftTop.x, y + toLeftTop.y, y - toLeftTop.y))
    {}
    ActivateObj(): gObj(),activeRectangle(nullptr){}
public :
    template <typename managerT>
    static EntityObj& newObj(managerT & m, const gMath::Crdinate& crd = gMath::Crdinate(0 , 0), const gMath::Angle& angle_ = 0.0){
       return basicObjInit<EntityObj>(m , crd, angle_);
    }
    template <typename managerT>
    static ActivateObj& newObj(managerT & m, const gMath::Crdinate& crd,const gMath::Angle& angle_, ActiveRectangle& rect){
       ActivateObj& t =  basicObjInit<EntityObj>(m , crd, angle_);
       t.activeRectangle = &rect;
       return t;}
   
};


#endif
