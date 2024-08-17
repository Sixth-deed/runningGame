#ifndef GAME_CONSTRAINT
#define GAME_CONSTRAINT
#include "lib/ObjPool.h"
#include "BaseObj.h"
#include "lib/Gmath.h"
#include <stack>
#include <utility>

enum ConstraintType
{
    CONTACTConstraint,
    POSITIONConstraint
};

enum ConstraintState
{
    NewlyAdded,
    OnGoing,
    Seperating
};


//////////////////////
//////Managers///////
/////////////////////

// 编译前封装好复杂类型的约束管理器
class wCstManager;

//辅助解析工作的双向链表
//这是一个虚基类，只提供必要的接口
class SolveChain;

enum SolverType{
    NormalSolver,
    VelAndPosSolver,

    SolverTypeCnt
};
//SolveChain的派生类
class NormalSolver;
class VelAndPosSolver;


template <typename... cstTypes>
class ConstraintManager
{
protected:
    std::stack<PhysicsObj *> toIntegrate;
    ObjectManager<cstTypes...> cstAllocManager;
    Constraint *curHead = nullptr;
    gMath::Graph <EntityObj*> cstGraph;

    SolveChain* chains[(size_t)SolverType::SolverTypeCnt];
public:
    template <typename cstType, typename... Args>
    static cstType *newConstraint(Args... args);
    template <typename cstType>
    void releaseConstraint(cstType *cst);
    inline void integrateAll();
    ConstraintManager() :toIntegrate(), cstAllocManager({5 , 5}), curHead(nullptr), cstGraph() {
        chains[(size_t)SolverType::NormalSolver] = new NormalSolver(this);
        chains[(size_t)SolverType::VelAndPosSolver] = new VelAndPosSolver(this);
    }
    ConstraintManager(ObjectManager<cstTypes...> &cstAllocManager_, gMath::Graph <EntityObj*>&& cstGraph_) : cstAllocManager(cstAllocManager_), cstGraph(cstGraph_) {}
    void addObjToIntegrateList(PhysicsObj *obj);
    void checkAndUpdate(EntityObj* ett1p, EntityObj* ett2p);
    void clear();

    bool ConstarintExist(EntityObj* ett1, EntityObj* ett2){return cstGragh->containsEdge(ett1, ett2);}   

    ~ConstraintManager() { clear(); }

    void solveAll();
    void updateAll();
};

class wCstManager : public ConstraintManager<NormalContactConstraint, PhysicsContactConstraint>
{
    public :
    wCstManager() : ConstraintManager<NormalContactConstraint, PhysicsContactConstraint>() {}
};


///////////////////
////Constraints////
///////////////////


class Constraint
{
protected:
    //友元类
    template <typename U>
    friend class ObjPool;

    friend class wCstManager;
    friend class SolveChain;

    static const ConstraintType cstType;
    EntityObj *ett1;
    EntityObj *ett2;
    ConstraintState cstState = ConstraintState::NewlyAdded;
    // 用于标记哪个ett是movable的
    short moveableFlag;

    // 直接在双向链表中管理，利于动态分配
    Constraint *next = nullptr;
    Constraint *prev = nullptr;
    Constraint() = default; 
    Constraint(EntityObj *ett1_, EntityObj *ett2_) : ett1(ett1_), ett2(ett2_), moveableFlag((ett1->movable ? 1 : 0) + (ett2->movable ? 2 : 0)) {}

    static wCstManager* cstManager;
public:
    static void setCstManager(wCstManager *cstManager_) { cstManager = cstManager_; }
    virtual void solve() = 0;
    virtual void update() = 0;
    

    virtual void release() ;

    ConstraintType getType() const { return cstType; }
    ConstraintState getState() const { return cstState; }
    void setState(ConstraintState state) { cstState = state; }

    // 应当提供这样成对的方法，后者只比前者多manager参数
    virtual void initializeConstraint( EntityObj *const ett1_, EntityObj *const ett2_);
    static Constraint *newConstraint(wCstManager &cstManager, EntityObj *ett1_, EntityObj *ett2_);
    Constraint* getNext()    { return next; }
    Constraint* getPrev()    { return prev; }
};


class ContactConstraint : public Constraint
{
protected:
    static const ConstraintType cstType = ConstraintType::CONTACTConstraint;
    // 由两个对象重心指向接触点的向量
    gMath::tVector toCp1;
    gMath::tVector toCp2;
    // 接触法线
    // 这里的接触法线一定是由ett2指向ett1
    gMath::tVector normal;
    ContactConstraint() = default;
public:
    ContactConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&toCp1_, const gMath::tVector &&toCp2_, const gMath::tVector &&normal_) : Constraint(ett1_, ett2_), toCp1(toCp1_), toCp2(toCp2_), normal(normal_) {}
    void update() override;
    void solve() override;
    virtual bool seperated();

    void initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&toCp1_, const gMath::tVector &&toCp2_, const gMath::tVector &&normal_);
    static ContactConstraint* newConstraint(wCstManager &cstManager, EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&toCp1_, const gMath::tVector &&toCp2_, const gMath::tVector &&normal_);
};

mOptional<clsn::CollisionLocal> isReallyIntersects(EntityObj &b1, EntityObj &b2);


class NormalContactConstraint : public ContactConstraint
{
private:
    NormalContactConstraint() = default;
public:
    NormalContactConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_) : ContactConstraint(ett1_, ett2_, std::move(t1cp_), std::move(t2cp_), std::move(normal_)) {}
    void initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
    NormalContactConstraint *newConstraint(wCstManager &cstManager, EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
};

class PhysicsContactConstraint : public ContactConstraint
{
private:
    // 临时
    gMath::tVector tangent;
    double normal_inv_effectiveMass;
    double tangent_inv_effectiveMass;
    gMath::tVector dv;
    // 长期
    double oldNormalImpulse;
    double oldTangentImpulse;
    PhysicsObj *pt1;
    PhysicsObj *pt2;
    double friction;
    double restitution;

    static const double posBiasFactor;
    static const double maxPenetraintion;
public:
    void initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
    PhysicsContactConstraint* newConstraint(wCstManager &cstManager,EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
    PhysicsContactConstraint(EntityObj *ett1_, EntityObj *ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
    void solve_v();
    void solve_p();
    void init();
    PhysicsContactConstraint* getNext() { return reinterpret_cast<PhysicsContactConstraint *>(next); }
    PhysicsContactConstraint* getPrev() { return reinterpret_cast<PhysicsContactConstraint *>(prev); }
};



class SolveChain{
public:
    virtual void solveAll() = 0;
    virtual void updateAll(); 
    void insert(Constraint* c);
    SolveChain(wCstManager*const manager):cstManager(manager){}
    ~SolveChain();
protected:
    wCstManager *const cstManager = nullptr;
    Constraint* head = nullptr;
};
class NormalSolver: public SolveChain{
public:
    NormalSolver(wCstManager*const manager):SolveChain(manager){}
    void solveAll() override;
};
class VelAndPosSolver : public SolveChain{
private:
    static const int vIterations;
    static const int pIterations;
public :
    VelAndPosSolver(wCstManager*const manager):SolveChain(manager){}
    void solveAll() override;
};
////////////////////
//inline functions//
////////////////////
inline void ContactConstraint::solve()
{
    ett1->CollisionAct(*ett2);
    ett2->CollisionAct(*ett1);
}
inline void Constraint::release()
{
    cstManager->releaseConstraint(this);
}
inline void Constraint::initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_)
{
    ett1 = ett1_;
    ett2 = ett2_;
    moveableFlag = (ett1->movable ? 1 : 0) + (ett2->movable ? 2 : 0);
}
inline void ContactConstraint::initializeConstraint(EntityObj * const ett1_, EntityObj * const ett2_, const gMath::tVector &&toCp1_, const gMath::tVector &&toCp2_, const gMath::tVector &&normal_)
{
    Constraint::initializeConstraint(ett1_, ett2_);
    toCp1 = std::move(toCp1_);
    toCp2 = std::move(toCp2_);
    normal = std::move(normal_);
}
inline ContactConstraint *ContactConstraint::newConstraint(wCstManager &cstManager, EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&toCp1_, const gMath::tVector &&toCp2_, const gMath::tVector &&normal_)
{
    return cstManager.template newConstraint<ContactConstraint>(ett1_, ett2_, std::move(toCp1_), std::move(toCp2_), std::move(normal_));
}
template <typename... cstTypes>
template <typename cstType>
inline void ConstraintManager<cstTypes...>::releaseConstraint(cstType *cst)
{
    if (cst->prev != nullptr)
    {
        cst->prev->next = cst->next;
        cst->next->prev = cst->prev;
    }
    else
    {
        curHead = cst->next;
        cst->next->prev = nullptr;
    }
    cstGraph->removeEdge(cst->ett1, cst->ett2);
    cstAllocManager.template release<cstType>(cst);
}

template <typename... cstTypes>
inline void ConstraintManager<cstTypes...>::addObjToIntegrateList(PhysicsObj *obj)
{
    toIntegrate.push(obj);
}



template <typename... cstTypes>
template <typename cstType, typename... Args>
inline cstType *ConstraintManager<cstTypes...>::newConstraint(Args... args)
{
    cstType *pt = cstAllocManager.template acquire<cstType>();
    pt->initializeConstraint(args...);

    Constraint *cpt const = reinterpret_cast<Constraint *>(pt);
    cpt->next = curHead;
    curHead = cpt;

    cstGraph->addEdge(pt->ett1, pt->ett2);
    return pt;
}


inline Constraint *Constraint::newConstraint(wCstManager &cstManager, EntityObj *ett1_, EntityObj *ett2_)
{
    return cstManager.template newConstraint<Constraint>(ett1_, ett2_);
}

inline void PhysicsContactConstraint::initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_)
{
    ContactConstraint::initializeConstraint(ett1_, ett2_, std::move(t1cp_), std::move(t2cp_), std::move(normal_));
    pt1 = reinterpret_cast<PhysicsObj *>(ett1_);
    pt2 = reinterpret_cast<PhysicsObj *>(ett2_);
    init();
    oldNormalImpulse = 0;
    oldTangentImpulse = 0;
    friction = pt1->getFriction() * pt2->getFriction();
    restitution = pt1->getRestitution() * pt2->getRestitution();
    restitution = restitution > MINIMAL_RESTITUTION ? restitution : 0;
}

inline PhysicsContactConstraint *PhysicsContactConstraint::newConstraint(wCstManager &cstManager, EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_)
{
    return cstManager.template newConstraint<PhysicsContactConstraint>(ett1_, ett2_, std::move(t1cp_), std::move(t2cp_), std::move(normal_));
}

inline PhysicsContactConstraint::PhysicsContactConstraint(EntityObj *ett1_, EntityObj *ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_) : 
    ContactConstraint(ett1_, ett2_, std::move(t1cp_), std::move(t2cp_), std::move(normal_)),
    pt1(reinterpret_cast<PhysicsObj *>(ett1)), pt2(reinterpret_cast<PhysicsObj *>(ett2))
{
    init();
    oldNormalImpulse = 0;
    oldTangentImpulse = 0;
    friction = pt1->getFriction() * pt2->getFriction();
    restitution = pt1->getRestitution() * pt2->getRestitution();
    restitution = restitution > MINIMAL_RESTITUTION ? restitution : 0;
}


inline mOptional<clsn::CollisionLocal>  isReallyIntersects(EntityObj &b1, EntityObj &b2)
{
    return clsn::isReallyIntersects(b1.getCrd(), b1.getCollisionBox(), b1.getAngle(), b2.getCrd(), b2.getCollisionBox(), b2.getAngle());
}
inline void NormalContactConstraint::initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_)
{
    ContactConstraint::initializeConstraint(ett1_, ett2_, std::move(t1cp_), std::move(t2cp_), std::move(normal_));
}
inline NormalContactConstraint* NormalContactConstraint::newConstraint(wCstManager &cstManager, EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_){
    return cstManager.template newConstraint<NormalContactConstraint>(ett1_, ett2_, std::move(t1cp_), std::move(t2cp_), std::move(normal_));
}

void SolveChain::insert(Constraint *c)
{
    c->next = head;
    head ->prev = c;
    head = c;
}
#endif