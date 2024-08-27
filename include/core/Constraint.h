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
class mNormalSolver;

class mVelAndPosSolver;

class Constraint;
class ContactConstraint;
class NormalContactConstraint;
class PhysicsContactConstraint;


template <typename... cstTypes>
class ConstraintManager
{
protected:
    std::stack<PhysicsObj *> toIntegrate;
    ObjectManager<cstTypes...> cstAllocManager;
    gMath::Graph <EntityObj*, Constraint*> cstGraph;

    SolveChain* chains[(size_t)SolverType::SolverTypeCnt];
public:
    template <typename cstType, typename... Args>
    cstType *newConstraint(Args... args);
    template <typename cstType>
    void releaseConstraint(cstType *cst);
    inline void integrateAll();
    ConstraintManager(); 
    ConstraintManager(ObjectManager<cstTypes...> &cstAllocManager_, gMath::Graph <EntityObj*,Constraint*>&& cstGraph_) : cstAllocManager(cstAllocManager_), cstGraph(cstGraph_) {}
    void addObjToIntegrateList(PhysicsObj *obj);
    void checkAndUpdate(EntityObj* ett1p, EntityObj* ett2p);
    void clear();

    bool ConstarintExist(EntityObj* ett1, EntityObj* ett2){return cstGraph.containsEdge(ett1, ett2);}   

    ~ConstraintManager() { clear(); }

    void solveAll();
    void updateAll();
    void renewDvFor(EntityObj* ett){
        for (auto& pair : cstGraph[ett]){
            Constraint* const cst = pair.second;
            cst->reNewDv();
        }
    }
};

class wCstManager : public ConstraintManager<ContactConstraint ,NormalContactConstraint, PhysicsContactConstraint>
{
    public :
    wCstManager(); };


///////////////////
////Constraints////
///////////////////


class Constraint
{
protected:
    //友元类
    template <typename U>
    friend class ObjPool;
    template <typename... TS>
    friend class ObjectManager;
    template <typename... Ts>
    friend class ConstraintManager;
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
    Constraint(EntityObj *ett1_, EntityObj *ett2_) : ett1(ett1_), ett2(ett2_), moveableFlag((ett1->isMovable() ? 1 : 0) + (ett2->isMovable() ? 2 : 0)) {}

    static wCstManager* cstManager;
public:
    static const SolverType solverType = SolverType::NormalSolver;
    static void setCstManager(wCstManager *cstManager_) { cstManager = cstManager_; }
    virtual void solve() = 0;
    virtual void update() = 0;
    
    Constraint() = default; 
    virtual ~Constraint() = default;
    virtual void release() = 0 ;

    ConstraintType getType() const { return cstType; }
    ConstraintState getState() const { return cstState; }
    void setState(ConstraintState state) { cstState = state; }

    void initializeConstraint( EntityObj *const ett1_, EntityObj *const ett2_);
    Constraint* getNext()    { return next; }
    Constraint* getPrev()    { return prev; }
    //给VelAndPosSolver调用的接口
    virtual void solve_v() {}
    virtual void solve_p() {}
    virtual void warmStart(){}
    virtual void reNewDv() {};
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
public:

    ContactConstraint() = default;
    ~ContactConstraint() = default;
    ContactConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&toCp1_, const gMath::tVector &&toCp2_, const gMath::tVector &&normal_) : Constraint(ett1_, ett2_), toCp1(toCp1_), toCp2(toCp2_), normal(normal_) {}
    void update() override;
    void solve() override;
    virtual bool seperated();
    void release() override{}
    void initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&toCp1_, const gMath::tVector &&toCp2_, const gMath::tVector &&normal_);
    static ContactConstraint* newConstraint(wCstManager &cstManager, EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&toCp1_, const gMath::tVector &&toCp2_, const gMath::tVector &&normal_);
};

mOptional<clsn::CollisionLocal> isReallyIntersects(EntityObj &b1, EntityObj &b2);


class NormalContactConstraint : public ContactConstraint
{
public:
    template <typename... Ts>
    friend class ConstraintManager;

    static const SolverType solverType = SolverType::NormalSolver;
    NormalContactConstraint() = default;
    ~NormalContactConstraint() = default;
    NormalContactConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_) : ContactConstraint(ett1_, ett2_, std::move(t1cp_), std::move(t2cp_), std::move(normal_)) {}
    void initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
    static NormalContactConstraint *newConstraint(wCstManager &cstManager, EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
    void release();
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

    void renewDv();
public:

    static const SolverType solverType = SolverType::VelAndPosSolver;
    PhysicsContactConstraint() = default;
    ~PhysicsContactConstraint() = default;
    void initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
    static PhysicsContactConstraint* newConstraint(wCstManager &cstManager,EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
    PhysicsContactConstraint(EntityObj *ett1_, EntityObj *ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_);
    void solve_v() override;
    void solve_p() override;
    void warmStart() override;
    void init();
    void release();
    
};



class SolveChain{
public:
    template <typename... Ts>
    friend class ConstraintManager;

    virtual void solveAll() = 0;
    virtual void updateAll(); 
    virtual void insert(Constraint* c);


    SolveChain(wCstManager*const manager):cstManager(manager){}
    virtual ~SolveChain();
protected:
    wCstManager *const cstManager = nullptr;
    //SEETAG
    Constraint* head = nullptr;
};
class mNormalSolver: public SolveChain{
public:
    mNormalSolver(wCstManager*const manager):SolveChain(manager){}
    void solveAll() override;
};

class mVelAndPosSolver : public SolveChain{
private:
    static const int vIterations;
    static const int pIterations;

public :
    mVelAndPosSolver(wCstManager*const manager):SolveChain(manager){}
    void solveAll() override;
};
////////////////////
//inline functions//
////////////////////
inline void ContactConstraint::solve()
{
    if ( cstState == ConstraintState::OnGoing){
        ett1->CollisionAct(*ett2);
        ett2->CollisionAct(*ett1);
    }
}
inline void NormalContactConstraint::release()
{
    
    cstManager->releaseConstraint(this);
}

inline void PhysicsContactConstraint::release()
{
    
    cstManager->releaseConstraint(this);
}

inline void Constraint::initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_)
{
    ett1 = ett1_;
    ett2 = ett2_;
    moveableFlag = (ett1->isMovable() ? 1 : 0) + (ett2->isMovable() ? 2 : 0);
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
        chains[cstType::solverType]->head = cst->next;
        if (cst->next != nullptr)
            cst->next->prev = nullptr;
    }
    cstGraph.removeEdge(cst->ett1, cst->ett2);
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
    pt->initializeConstraint(std::forward<Args>(args)...);

    pt->next = chains[cstType::solverType]->head;
    chains[cstType::solverType]->head = pt;

    cstGraph.addEdge(pt->ett1, pt->ett2, pt);
    return pt;
}



inline void PhysicsContactConstraint::initializeConstraint(EntityObj *const ett1_, EntityObj *const ett2_, const gMath::tVector &&t1cp_, const gMath::tVector &&t2cp_, const gMath::tVector &&normal_)
{
    ContactConstraint::initializeConstraint(ett1_, ett2_, std::move(t1cp_), std::move(t2cp_), std::move(normal_));

    //PT_CAST ETT -> PHY
    pt1 = dynamic_cast<PhysicsObj *>(ett1_);
    pt2 = dynamic_cast<PhysicsObj *>(ett2_);
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
    //PT_CAST ETT -> PHY
    pt1(dynamic_cast<PhysicsObj *>(ett1)), pt2(dynamic_cast<PhysicsObj *>(ett2))
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

inline void SolveChain::insert(Constraint *c)
{
    c->next = head;
    head ->prev = c;
    head = c;
}

template <typename... Ts>
inline ConstraintManager<Ts...>::ConstraintManager() :toIntegrate(), cstAllocManager({5 , 5}), cstGraph() {
    }

inline wCstManager::wCstManager() : ConstraintManager<ContactConstraint,NormalContactConstraint, PhysicsContactConstraint>() {

        chains[(size_t)SolverType::NormalSolver] = static_cast<SolveChain*>(new mNormalSolver(this));
        chains[(size_t)SolverType::VelAndPosSolver] = static_cast<SolveChain*>(new mVelAndPosSolver(this));
}

//////////////////////
//template functions//
/////////////////////
template <typename... cstTypes>
inline void ConstraintManager<cstTypes...>::checkAndUpdate(EntityObj *ett1p, EntityObj *ett2p)
{
    if (ett1p == ett2p) return;
    switch (whatCollide(*ett1p, *ett2p))
    {
    case CollisionType::NoCollision:
        break;
    case CollisionType::NormalCollision:
        if (isOuterIntersects(*ett1p, *ett2p) && !cstGraph.containsEdge(ett1p, ett2p) && !(ett1p->isSleep() && ett2p->isSleep()))
        {
            mOptional<clsn::CollisionLocal> cl = isReallyIntersects(*ett1p, *ett2p);
            if (cl)
            {
                NormalContactConstraint::newConstraint(*static_cast<wCstManager*>(this), ett1p, ett2p, std::move(cl.value().toBody1_cp), std::move(cl.value().toBody2_cp), std::move(cl.value().normal));
            }
        }
    case CollisionType::PhysicsCollision:
        if (isOuterIntersects(*ett1p, *ett2p) && !cstGraph.containsEdge(ett1p, ett2p) && !(ett1p->isSleep() && ett2p->isSleep()))
        {
            mOptional<clsn::CollisionLocal> cl = isReallyIntersects(*ett1p, *ett2p);
            if (cl)
            {
                PhysicsContactConstraint::newConstraint(*static_cast<wCstManager*>(this), ett1p, ett2p, std::move(cl.value().toBody1_cp), std::move(cl.value().toBody2_cp), std::move(cl.value().normal));
            }
        }
    }
}

template <typename... cstTypes>
void ConstraintManager<cstTypes...>::integrateAll()
{
    while (!toIntegrate.empty())
    {
        toIntegrate.top()->Integrate(1);
        toIntegrate.pop();
    }
}

template <typename... cstTypes>
void ConstraintManager<cstTypes...>::clear()
{
    cstAllocManager.clear();
    for (auto solver_p : chains)
    {
        delete solver_p;
    }
    
}

template <typename... cstTypes>
void ConstraintManager<cstTypes...>::solveAll()
{
    for (auto solver_p : chains)
    {
        solver_p->solveAll();
    }
}

template <typename... cstTypes>
void ConstraintManager<cstTypes...>::updateAll()
{
    for (auto solver_p : chains)
    {
        solver_p->updateAll();
    }
}


inline void PhysicsContactConstraint::renewDv()
{
    const gMath::tVector &wv1 = toCp1.cross((pt1)->getAngleVelocity());
    const gMath::tVector &wv2 = toCp2.cross((pt2)->getAngleVelocity());
    const gMath::tVector &v1 = pt1->getVelocity();
    const gMath::tVector &v2 = pt2->getVelocity();
    dv = wv1 + v1 - wv2 - v2;
}

#endif
