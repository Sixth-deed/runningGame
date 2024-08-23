#ifndef BASIC_GAME_OBJECT
#define BASIC_GAME_OBJECT
#include "lib/Gmath.h"
#include "gObj.h"
#include "lib/idHandler.h"
#include "lib/Constants.h"
#include <csignal>
#include <cstdlib>
#include <memory>
#include <new>
#include <unordered_map>
#include <utility>
// 实体类型
enum EntityType
{
    MainEntity,
    StableEntity,
    PassiveMoveEntity,
    OptimisicMoveEntity,
    ForMainEntity
};
// 碰撞类型
enum CollisionType
{
    NoCollision,
    NormalCollision,
    PhysicsCollision
};
// 只有在这个图中有边的类型之间才会被检测碰撞
constexpr CollisionType WillCollide[5][5]{
    {(CollisionType)0, (CollisionType)2, (CollisionType)2, (CollisionType)2, (CollisionType)1},
    {(CollisionType)2, (CollisionType)0, (CollisionType)2, (CollisionType)2, (CollisionType)0},
    {(CollisionType)2, (CollisionType)2, (CollisionType)2, (CollisionType)2, (CollisionType)0},
    {(CollisionType)2, (CollisionType)2, (CollisionType)2, (CollisionType)0, (CollisionType)0},
    {(CollisionType)1, (CollisionType)0, (CollisionType)0, (CollisionType)0, (CollisionType)0}};

#define GRID_NEEDED
#define PRIORITY_LEVEL5
// 实体对象，拥有碰撞体积，可以参与碰撞体积检测
// 发生有效碰撞后执行CollisionAct()方法
class EntityObj;

#define GRID_NEEDED
#define PRIORITY_LEVEL1
// 拥有act方法的对象
// 重载act时不要忘了调用基类的act
class ActObj;

// 拥有act,addNewAction和removeAction方法的对象
// 与ActObj不同的是，它可以在运行时动态地添加行为
class DynamicActObj;

// 可移动的对象
// 有速度和加速度属性
class MoveObj;

// 可旋转的对象
// 有中心点和角速度属性
class RotateObj;

// 有物理属性的对象
// 无速度和加速度属性
// 无旋转属性
class PhysicsObj;

// 有监听器的对象
// 即有可能需要在运行时判定某个条件以执行代码的对象,有addLiscenerOn()和removeLiscenerOn方法
// 与ExtraActObj的主要不同是自带条件判断，异步执行
class LiscenerObj;

// 保持片区活跃的对象
// 保持对对象activeRectangle范围内所有对象的更新
class ActivateObj;

#include "Collision.h"
/**
 * @class EntityObj
 * @brief Represents an entity object with a collision box.
 * @inherits gObj
 */
class EntityObj : virtual public gObj
{
protected:
    // 碰撞箱
    clsn::CollisionBox clsnBox;
    /**
     * @brief Default constructor.
     * Initializes the collision box to nullptr.
     */
    EntityObj() : gObj(), clsnBox() {}
    static const EntityType etype;

public:
    /**
     * @brief Returns a reference to the collision box associated with the entity object.
     * @return clsn::CollisionBox&
     */
    clsn::CollisionBox &getCollisionBox() { return clsnBox; }
    /**
     * @brief Returns a constant reference to the collision box associated with the entity object.
     * @return const clsn::CollisionBox&
     */
    const clsn::CollisionBox &get_c_CollisionBox() const { return clsnBox; }
    /**
     * @brief Pure virtual function to handle collisions with another entity object.
     * Must be implemented by derived classes.
     * @param another_obj The other entity object involved in the collision.
     * @return bool
     */
    virtual bool CollisionAct(EntityObj &another_obj) = 0;

    /**
     * @brief Static factory function to create a new entity object with the specified coordinates and angle.
     * @tparam managerT The type of the manager object.
     * @param m The manager object.
     * @param crd The coordinates of the entity object.
     * @param angle_ The angle of the entity object.
     * @return EntityObj&
     */
/*    template <typename managerT>
    static EntityObj &newObj(managerT &m, const gMath::Crdinate &crd = gMath::Crdinate(0, 0), const gMath::Angle &angle_ = 0.0)
    {
        return basicObjInit<EntityObj>(m, crd, angle_);
    }*/
    /**
     * @brief Static factory function to create a new entity object with the specified coordinates, angle, and collision box.
     * @tparam managerT The type of the manager object.
     * @param m The manager object.
     * @param crd The coordinates of the entity object.
     * @param angle_ The angle of the entity object.
     * @param cl The collision box of the entity object.
     * @return EntityObj&
     */
    /*template <typename managerT>
    static EntityObj &newObj(managerT &m, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &cl)
    {
        EntityObj &t = basicObjInit<EntityObj>(m, crd, angle_);
        t.clsnBox = &cl;
        return t;
    }*/
    static void initObj(EntityObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &&cl){
        gObj::initObj(pt, crd, angle_);
        pt->clsnBox = std::move(cl);
    }
    /**
     * @brief Friend function to determine the collision type between two entity objects.
     * @param b1 The first entity object.
     * @param b2 The second entity object.
     * @return CollisionType
     */
    friend inline constexpr CollisionType whatCollide(const EntityObj &b1, const EntityObj &b2);
};


inline bool isOuterIntersects(const EntityObj &b1, const EntityObj &b2)
{
    return clsn::isOuterIntersect(b1.getCrd(), b1.get_c_CollisionBox(), b2.getCrd(), b2.get_c_CollisionBox());
}
inline constexpr CollisionType whatCollide(const EntityObj &b1, const EntityObj &b2)
{
    return WillCollide[static_cast<int>(b1.etype)][static_cast<int>(b2.etype)];
}

class ActObj : virtual public gObj
{
public:
    // 写入当前类的行为
    virtual void act() = 0;
    static void initObj(ActObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_){
        gObj::initObj(pt, crd, angle_);
    }
    template <typename managerT>
    static void newObj(managerT &m, const gMath::Crdinate &crd, const gMath::Angle &angle_){
        return basicObjInit<ActObj>(m, crd, angle_);
    }
    bool isSleeping = true;
};

class DynamicActObj : virtual public ActObj
{
protected:
    std::unordered_map<std::string, void (*)(DynamicActObj *)> *actions;
    DynamicActObj() : gObj(), actions(new std::unordered_map<std::string, void (*)(DynamicActObj *)>(1)) {}
    DynamicActObj(gMath::axisV x, gMath::axisV y) : gObj(x, y), actions(new std::unordered_map<std::string, void (*)(DynamicActObj *)>(1)) {}

public:
    void act() override
    {
        for (auto f : *actions)
        {
            f.second(this);
        }
    }
    void addNewAction(const std::string &funcName, void (*f)(DynamicActObj *))
    {
        (*actions)[funcName] = f;
    }
    void removeAction(const std::string &funcName)
    {
        (*actions).erase(funcName);
    }
    template <typename managerT>
    static DynamicActObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_ = 0.0)
    {
        return basicObjInit<DynamicActObj>(m, crd, angle_);
    }

    template <typename managerT, typename IteratorType>
    static DynamicActObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, IteratorType &begin, IteratorType &end)
    {
        DynamicActObj &t = basicObjInit<DynamicActObj>(m, crd, angle_);
        delete t.actions;
        t.actions = new std::unordered_map(begin, end);
        return t;
    }
    template <typename managerT>
    static DynamicActObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, std::unordered_map<std::string, void (*)(DynamicActObj *)> &acts)
    {
        DynamicActObj &t = basicObjInit<DynamicActObj>(m, crd, angle_);
        delete t.actions;
        t.actions = &acts;
        return t;
    }
    template <typename IteratorType>
    static void initObj(DynamicActObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_ ,IteratorType &begin, IteratorType &end){
        ActObj::initObj(pt, crd, angle_);
        delete pt->actions;
        pt->actions = new std::unordered_map(begin, end);
    }
    static void initObj(DynamicActObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, std::unordered_map<std::string, void (*)(DynamicActObj *)> &acts){
        ActObj::initObj(pt, crd, angle_);
        delete pt->actions;
        pt->actions = &acts;
    }
    ~DynamicActObj(){
        delete actions;
    }
};

class MoveObj : virtual public ActObj
{
    using tVector = gMath::tVector;

protected:
    tVector velocity;

    tVector acceleration;
    
    MoveObj() : ActObj(), velocity(0.0, 0.0), acceleration(0.0, 0.0) {}

    // 为0时直接删除对象
    // 为1时保留对象速度，直到对象再次在活跃范围中才更新对象位置
    // 为2时对象速度和加速度清零，再次在活跃范围中被加载
    // 为3时调用自定义的处理方法
    static const int onOffMethodSignal = 0;

    // 自定义的处理离开视野的方法
    // return 0表示删除对象，return 1会保留对象
    int s_onOffTackler() { return 0; }
    bool moved = false;
public:
    template <typename managerT>
    static MoveObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_ = 0.0)
    {
        return basicObjInit<MoveObj>(m, crd, angle_);
    }
    int onOffTackler()
    {
        switch (onOffMethodSignal)
        {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            setVelocity(tVector(0.0, 0.0));
            setAcceleration(tVector(0.0, 0.0));
            return onOffMethodSignal;
        case 3:
            return s_onOffTackler();
        }
    }
    template <typename managerT>
    static MoveObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, const tVector initialVelocity = tVector(0.0, 0.0), const tVector &initialAccelr = tVector(0.0, 0.0))
    {
        MoveObj &t = basicObjInit<MoveObj>(m, crd, angle_);
        t.velocity = initialVelocity;
        t.acceleration = initialAccelr;
        return t;
    }
    static void initObj(MoveObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, const tVector initialVelocity = tVector(0.0, 0.0), const tVector &initialAccelr = tVector(0.0, 0.0)){
        ActObj::initObj(pt, crd, angle_);
        pt->velocity = initialVelocity;
        pt->acceleration = initialAccelr;
    }
    const tVector &getVelocity() const
    {
        return velocity;
    }
    void setVelocity(const tVector &v)
    {
        velocity = v;
    }
    const tVector &getAcceleration() const
    {
        return acceleration;
    }
    void setAcceleration(const tVector &a)
    {
        acceleration = a;
    }
    void act() override
    {
        if (!isSleeping){
            velocity += acceleration;
            crd += velocity;
            moved = velocity.magnitude() >= MIN_VELOCITY_COUNT;
        }
        else
            moved = false;
    }
    bool isMoved() const{
        return moved;
    }
    static const bool movable = true;
};

class RotateObj : virtual public ActObj
{
protected:
    // 角速度
    gMath::Angle angleVelocity;
    // 角加速度
    gMath::Angle angleAcceleration;

    // 旋转中心默认为坐标点
    RotateObj() : ActObj(), angleVelocity(0.0),angleAcceleration(0.0) {}

public:
    void act() override
    {
        angle += angleVelocity;
    }
    template <typename managerT>
    static RotateObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_ = 0.0)
    {
        return basicObjInit<RotateObj>(m, crd, angle_);
    }
    template <typename managerT, typename IteratorType>
    static RotateObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0)
    {
        RotateObj &t = basicObjInit<MoveObj>(m, crd, angle_);
        t.angleVelocity = angleV;
        t.angleAcceleration = angleA;
        return t;
    }
    static void initObj(RotateObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0){
        ActObj::initObj(pt, crd, angle_);
        pt->angleVelocity = angleV;
        pt->angleAcceleration = angleA;
    }
    static const bool rotatable = true;

    gMath::Angle getAngleVelocity() const
    {
        return angleVelocity;
    }
    void setAngleVelocity(const gMath::Angle &a)
    {
        angleVelocity = a;
    }
    gMath::Angle getAngleAcceleration() const
    {
        return angleAcceleration;
    }
    void setAngleAcceleration(const gMath::Angle &a)
    {
        angleAcceleration = a;
    }
};
class ActiveRectangle : public gMath::mRectangle
{
private:
    bool moved = false;

public:
    static std::vector<ActiveRectangle *> rects;
    ActiveRectangle(gMath::axisV left, gMath::axisV right, gMath::axisV top, gMath::axisV bottom) : mRectangle(left, right, top, bottom)
    {

        rects.push_back(this);
    }
    void setNotMoved()
    {
        moved = false;
    }
    bool is_moved()
    {
        return moved;
    }
    /// @brief 更新活跃方格的位置. 更新前需要保证发生了位移
    /// @param v 方格移动的位移
    void Update(const gMath::tVector &v)
    {
        l += v.x;
        r += v.x;
        t += v.y;
        b += v.y;
        moved = true;
    }
    static void clear()
    {
        rects.clear();
    }
    static bool intersectsWith(const gMath::mRectangle& rec){
        for (auto A_rect : rects){
            if (A_rect->intersects(rec)){
                return true;
            }
        }
        return false;
    }
};


class ActivateObj : virtual public gObj
{
protected:
    ActiveRectangle *activeRectangle;
    //(x,y) 中心坐标
    // toLeftTop 由中心指向方形格左上角顶点的向量
    ActivateObj(gMath::axisV x, gMath::axisV y, gMath::tVector toLeftTop) : gObj(x, y),
                                                                            activeRectangle(new ActiveRectangle(x + toLeftTop.x, x - toLeftTop.x, y + toLeftTop.y, y - toLeftTop.y))
    {
    }
    ActivateObj() : gObj(), activeRectangle(nullptr) {}

public:
    template <typename managerT>
    static ActivateObj &newObj(managerT &m, const gMath::Crdinate &crd = gMath::Crdinate(0, 0), const gMath::Angle &angle_ = 0.0)
    {
        return basicObjInit<ActivateObj>(m, crd, angle_);
    }
    template <typename managerT>
    static ActivateObj &newObj(managerT &m, const gMath::Crdinate &crd, const gMath::Angle &angle_, ActiveRectangle &rect)
    {
        ActivateObj &t = basicObjInit<ActivateObj>(m, crd, angle_);
        t.activeRectangle = &rect;
        return t;
    }
    static void initObj(ActivateObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, ActiveRectangle &rect){
        gObj::initObj(pt, crd, angle_);
        pt->activeRectangle = &rect;
    }
    ~ActivateObj(){
        delete activeRectangle;
    }
};
class PhysicsEngine;
class PhysicsObj : virtual public EntityObj
{
public:/*    template <typename managerT>
    static PhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd = gMath::Crdinate(0, 0), const gMath::Angle &angle_ = 0.0)
    {
        return basicObjInit<PhysicsObj>(m, crd, angle_);
    }
    template <typename managerT>
    static PhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &cl,
                              double mass_ = 1.0, double friction_C_ = 0.0, double restitution_C_ = 1.0, bool graviityAffected_ = true, bool dragAffected_ = true)
    {
        PhysicsObj &t = basicObjInit<PhysicsObj>(m, crd, angle_);
        t.clsnBox = &cl;
        t.mass = mass_;
        t.inverseMass = mass_ ==0 ? 0: 1 / mass_;
        t.friction_C = friction_C_;
        t.restitution_C = restitution_C_;
        t.gravityAffected = graviityAffected_;
        t.dragAffected = dragAffected_;
        return t;
    }*/
    static void initObj(PhysicsObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &&cl,
                        double mass_ = 1.0, double friction_C_ = 0.0, double restitution_C_ = 1.0, bool graviityAffected_ = true, bool dragAffected_ = true){
                            EntityObj::initObj(pt, crd, angle_, std::move(cl));
                            pt->mass = mass_;
                            pt->inverseMass = mass_ ==0 ? 0: 1 / mass_;
                            pt->friction_C = friction_C_;
                            pt->restitution_C = restitution_C_;
                            pt->gravityAffected = graviityAffected_;
                            pt->dragAffected = dragAffected_;
                        }
    friend inline constexpr CollisionType whatCollide(const EntityObj &b1, const EntityObj &b2);
    static const bool isEntity = true;
    virtual void applyForceOnCenter(const gMath::tVector &force) = 0;
    virtual void applyImpulseOnCenter(const gMath::tVector &impulse) = 0;
    virtual void applyForceAtPoint(const gMath::tVector &force, const gMath::Crdinate &point) = 0;
    virtual void applyImpulseAtPoint(const gMath::tVector &impulse, const gMath::Crdinate &point) = 0;
    virtual void applyImpulse_v(const gMath::tVector &impulse, const gMath::tVector& r) = 0;
    virtual void Integrate(double dt) = 0;

    double getMass() const{
        return mass;
    }
    double getInverseMass() const{
        return inverseMass;
    }
    double getFriction() const{
        return friction_C;
    }
    double getRestitution() const{
        return restitution_C;
    }
    bool isGravityAffected() const{
        return gravityAffected;
    }
    bool isDragAffected() const{
        return dragAffected;
    }
    virtual void moveFix(const gMath::tVector&& ) = 0;
    virtual void rotateFix(const gMath::Angle&& ) = 0;
    virtual double getInverseInertia() const {return 0.0;};
    static void setPhysicsEngine(PhysicsEngine* p){
        mainPhysicsEngine = p;
    }
protected:
    // 质量
    double mass;
    //质量倒数
    double inverseMass;
    // 摩擦系数
    double friction_C;
    // 恢复系数
    double restitution_C;
    // 是否受全局重力影响
    bool gravityAffected;
    // 是否受阻尼影响（不是摩擦力）
    bool dragAffected;

    static PhysicsEngine* mainPhysicsEngine;

    PhysicsObj() : EntityObj() {}
    
    
};  
#endif
