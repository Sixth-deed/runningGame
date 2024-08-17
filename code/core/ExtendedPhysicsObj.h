#ifndef EXTENDED_PHYSICS_OBJ
#define EXTENDED_PHYSICS_OBJ

#include "BaseObj.h"
#include "lib/Gmath.h"
#include "PhysicsEngine.h"
/**
 * @brief The MovePhysicsObj class represents a physics object that can be moved.
 *
 * This class is a derived class of the PhysicsObj and MoveObj classes. It extends the functionality of
 * the PhysicsObj class by allowing the object to be moved. The MovePhysicsObj class provides
 * additional methods and properties that are specific to moving objects.
 *
 * The MovePhysicsObj class has the following properties:
 * - inverseMass: The inverse of the mass of the object.
 *
 * The MovePhysicsObj class has the following methods:
 * - MovePhysicsObj(): The default constructor.
 * - applyForce(const gMath::tVector &force): Applies a force to the object.
 * - applyImpulse(const gMath::tVector &impulse): Applies an impulse to the object.
 *
 * The MovePhysicsObj class has the following static properties:
 * - movable: Indicates whether the object can be moved.
 * - rotable: Indicates whether the object can be rotated.
 * - isEntity: Indicates whether the object is an entity.
 *
 * The MovePhysicsObj class has the following static methods:
 * - newObj(managerT &m, const gMath::Crdinate &crd = gMath::Crdinate(0, 0), const gMath::Angle &angle_ = 0.0):
 *   Creates a new instance of the MovePhysicsObj class with default values.
 * - newObj(managerT &m, const gMath::Crdinate &crd...):
 *   Creates a new instance of the MovePhysicsObj class with specific parameters.
 */
class MovePhysicsObj;

/**
 * The RotatePhysicsObj class represents a physics object that can rotate. It inherits from PhysicsObj and RotateObj and has properties such as inertia, inverse inertia, and flags indicating whether it is movable, rotatable, and an entity.
 */
class RotatePhysicsObj;

/**
 * @brief The StablePhysicsObj class represents a physics object that is not affected by forces and impulses.
 *
 * This class is a derived class of the PhysicsObj class. It extends the functionality of the PhysicsObj class by
 * making the object immune to forces and impulses. The StablePhysicsObj class provides a way to create objects that
 * are stationary or have a constant velocity and do not change their position or velocity in response to external
 * forces or impulses.
 *
 * The StablePhysicsObj class has the following properties:
 * - movable: Indicates whether the object can be moved.
 * - rotable: Indicates whether the object can be rotated.
 * - isEntity: Indicates whether the object is an entity.
 * - etype: Indicates the type of the entity.
 *
 * The StablePhysicsObj class has the following methods:
 * - applyForce(const gMath::tVector &force): Applies a force to the object.
 * - applyImpulse(const gMath::tVector &impulse): Applies an impulse to the object.
 *
 * @note The StablePhysicsObj class is designed to be used as a base class for other classes that require a constant
 *       position and velocity. Any changes to the position or velocity of the StablePhysicsObj object will not
 *       affect its behavior.
 */
class StablePhysicsObj;

/**
 * @brief The LiberalPhysicsObj class represents a physics object that is capable of movement and rotation.
 *
 * This class is a derived class of the MovePhysicsObj and RotatePhysicsObj classes. It extends the functionality
 * of the MovePhysicsObj and RotatePhysicsObj classes by making the object movable and rotatable. The
 * LiberalPhysicsObj class provides a way to create objects that are both movable and rotatable.
 *
 * The LiberalPhysicsObj class has the following properties:
 * - movable: Indicates whether the object can be moved.
 * - rotable: Indicates whether the object can be rotated.
 * - isEntity: Indicates whether the object is an entity.
 *
 * The LiberalPhysicsObj class has the following methods:
 * - act(): Acts on the object.
 * - applyForceOnCenter(const gMath::tVector &force): Applies a force to the object's center.
 * - applyImpulseOnCenter(const gMath::tVector &impulse): Applies an impulse to the object's center.
 * - applyForceAtPoint(const gMath::tVector &force, const gMath::Crdinate &point): Applies a force to a point.
 * - applyImpulseAtPoint(const gMath::tVector &impulse, const gMath::Crdinate &point): Applies an impulse to a point.
 */
class LiberalPhysicsObj;


/**
 * @brief Calculates the inertia of a shape based on its mass and geometry.
 */
double caculateInertia(double mass_, std::vector<gMath::tVector>* shape);

class MovePhysicsObj : virtual public PhysicsObj, virtual public MoveObj
{
protected:
    
    /**
     * Default constructor for the MovePhysicsObj class.
     *
     * Initializes the object with default values and calculates the inverse mass.
     *
     * @note This constructor is protected, as it is intended to be used by derived classes.
     *
     * @return None
     */
    MovePhysicsObj() : PhysicsObj(), MoveObj() {}
    
public:
    template <typename managerT>
    static MovePhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd = gMath::Crdinate(0, 0), const gMath::Angle &angle_ = 0.0)
    {
        return basicObjInit<MovePhysicsObj>(m, crd, angle_);
    }
    /**
         * @brief Constructs a new instance of the MovePhysicsObj class with all the specified parameters.
         *
         * This constructor is used to create a new instance of the MovePhysicsObj class with specific
         * parameters. It initializes the object with the provided position, angle, collision box, mass,
         * friction coefficient, restitution coefficient, gravity affecting, drag affecting, initial velocity,
         * and initial acceleration.
         *
         * @tparam managerT The type of the manager.
         * @param m The reference to the manager.
         * @param m The refearence to the manager.
         * @param crd The position of the object.
         * @param angle_ The angle of the object.
         * @param cl The collision box of the object.
         * @param mass_ The mass of the object.
         * @param friction_C_ The friction coefficient of the object.
         * @param restitution_C_ The restitution coefficient of the object.
         * @param graviityAffected_ Indicates whether the object is affected by gravity.
         * @param dragAffected_ Indicates whether the object is affected by drag.
         * @param initialVelocity The initial velocity of the object.
         * @param initialAccelr The initial acceleration of the object.
         *
         * @return A reference to the newly created MovePhysicsObj object.
         */
    template <typename managerT>
    static MovePhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd, const gMath::Angle &angle_, 
                            clsn::CollisionBox &cl,
                              double mass_ = 1.0, double friction_C_ = 0.0, double restitution_C_ = 1.0, bool graviityAffected_ = true, bool dragAffected_ = true,
                               const gMath::tVector &initialVelocity = gMath::tVector(0.0, 0.0), const gMath::tVector &initialAccelr = gMath::tVector(0.0, 0.0)
    );
    static void initObj(MovePhysicsObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &cl,
                        double mass_ = 1.0, double friction_C_ = 0.0, double restitution_C_ = 1.0, bool graviityAffected_ = true, bool dragAffected_ = true,
                        const gMath::tVector &initialVelocity = gMath::tVector(0.0, 0.0), const gMath::tVector &initialAccelr = gMath::tVector(0.0, 0.0));
    inline void applyForceOnCenter(const gMath::tVector &force) override;
    inline void applyImpulseOnCenter(const gMath::tVector &impulse) override;
    inline void applyForceAtPoint( const gMath::tVector &force, const gMath::Crdinate &point) override;
    inline void applyImpulseAtPoint(const gMath::tVector &impulse,const gMath::Crdinate &point) override;
    inline void applyImpulse_v(const gMath::tVector &impulse, const gMath::tVector &r) override;
    static const bool movable = true;
    static const bool rotable = false;
    static const bool isEntity = true;
    inline void act() override;
    inline void Integrate(double dt) override;
    void moveFix(const gMath::tVector &&fix) override;
    void rotateFix(const gMath::Angle &&anglefix) override{}
};


template <typename managerT>
inline MovePhysicsObj &MovePhysicsObj::newObj(managerT &m, const gMath::Crdinate &crd, const gMath::Angle &angle_, 
                            clsn::CollisionBox &cl,
                              double mass_ , double friction_C_ , double restitution_C_ , bool graviityAffected_, bool dragAffected_ ,
                               const gMath::tVector &initialVelocity, const gMath::tVector &initialAccelr)
{
        
    
        MovePhysicsObj &t = m.template acquire<MovePhysicsObj>();
        initObj(&t, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, initialVelocity, initialAccelr);
        return t;
}

inline void MovePhysicsObj::initObj(MovePhysicsObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &cl, double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_, const gMath::tVector &initialVelocity, const gMath::tVector &initialAccelr)
{
    PhysicsObj::initObj(pt, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_);
    MoveObj::initObj(pt, crd, angle_, initialVelocity, initialAccelr);
}

inline void MovePhysicsObj::applyForceOnCenter(const gMath::tVector &force)
{
    acceleration += force * inverseMass;
    isSleeping = false;
}
inline void MovePhysicsObj::applyImpulseOnCenter(const gMath::tVector &impulse)
{
    velocity += impulse * inverseMass;
    isSleeping = false;
}

inline void MovePhysicsObj::applyForceAtPoint(const gMath::tVector &force, const gMath::Crdinate &point)
{
    this->acceleration += force * inverseMass;
    isSleeping = false;
}

inline void MovePhysicsObj::applyImpulseAtPoint(const gMath::tVector &impulse,const gMath::Crdinate &point)
{
    velocity += impulse * inverseMass;
    isSleeping = false;
}

inline void MovePhysicsObj::applyImpulse_v(const gMath::tVector &impulse, const gMath::tVector &r)
{
    velocity += impulse * inverseMass;
    isSleeping = false;
}

inline void MovePhysicsObj::act()
{
    if (gravityAffected){
        acceleration = mainPhysicsEngine->GLOBAL_GRAVITY;
    }
    velocity += acceleration;
    if (dragAffected){
        velocity *= 1/(1+ mainPhysicsEngine -> GLOBAL_LINEAR_DAMPING);
    }
    mainPhysicsEngine -> getCstManager() ->addObjToIntegrateList(this);
}

inline void MovePhysicsObj::Integrate(double dt)
{
    crd += velocity * dt;
}

inline void MovePhysicsObj::moveFix(const gMath::tVector &&fix)
{
    crd += fix;
}

class StablePhysicsObj: virtual public PhysicsObj
{
public:
    /**
     * @brief Applies a force to the object.
     *
     * This method does not affect the object's position or velocity.
     *
     * @param force The force to be applied.
     */
    void applyForceOnCenter(const gMath::tVector &force) override{}

    /**
     * @brief Applies an impulse to the object.
     *
     * This method does not affect the object's position or velocity.
     *
     * @param impulse The impulse to be applied.
     */
    void applyImpulseOnCenter(const gMath::tVector &impulse) override{}
    void applyForceAtPoint(const gMath::tVector &force, const gMath::Crdinate &point) override{}
    void applyImpulseAtPoint(const gMath::tVector &impulse,const gMath::Crdinate &point) override{}
    void applyImpulse_v(const gMath::tVector &impulse, const gMath::tVector &r) override{}
    void Integrate(double dt) override{}
    /// Indicates whether the object can be moved.
    static const bool movable = false;

    /// Indicates whether the object can be rotated.
    static const bool rotable = false;

    /// Indicates whether the object is an entity.
    static const bool isEntity = true;

    /// Indicates the type of the entity.
    static const  EntityType etype = EntityType::StableEntity;
    template <typename managerT>
    static StablePhysicsObj& newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_ = 0.0){
        return basicObjInit<StablePhysicsObj>(m, crd, angle_);
    }
    template <typename managerT>
    static StablePhysicsObj& newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &cl, 
    double mass_ =0.0, double friction_C_ = 0.0, double restitution_C_ =1.0, bool graviityAffected_ =false, bool dragAffected_ = false){
        StablePhysicsObj &t = m.template acquire<StablePhysicsObj>();
        initObj(&t, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_);
        return t;
    }
    static void initObj(StablePhysicsObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &cl,
    double mass_ =0.0, double friction_C_ = 0.0, double restitution_C_ =1.0, bool graviityAffected_ =false, bool dragAffected_ = false){
        PhysicsObj::initObj(pt, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_);
    }
    void moveFix(const gMath::tVector &&fix) override{}
    void rotateFix(const gMath::Angle &&anglefix) override{}
};


class RotatePhysicsObj : virtual public PhysicsObj, virtual public RotateObj{
protected:
    //转动惯量
    double Inertia;
    //转动惯量的倒数
    double inverseInertia;

    RotatePhysicsObj() : PhysicsObj() , RotateObj(), Inertia(1.0), inverseInertia(1.0) {}
public:

    static const bool movable = false;

    static const bool rotable = true;

    static const bool isEntity = true;

    /**
     * @brief Constructs a new instance of the RotatePhysicsObj class with default values.
     *
     * This constructor is used to create a new instance of the RotatePhysicsObj class with default values.
     *
     * @tparam managerT The type of the manager.
     * @param m The reference to the manager.
     * @param crd The position of the object.
     * @param angle_ The angle of the object.
     *
     * @return A reference to the newly created RotatePhysicsObj object.
     */
    template <typename managerT>
    static RotatePhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_ = 0.0)
    {
        return basicObjInit<RotatePhysicsObj>(m, crd, angle_);
    }

    template <typename managerT>
    static RotatePhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &cl,
                                    double mass_ = 1.0, double friction_C_ = 0.0, double restitution_C_ = 1.0,
                                    bool graviityAffected_ = true, bool dragAffected_ = true, 
                                    const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0,double inertia_ = 1.0);
    static void initObj(RotatePhysicsObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &cl,
                                    double mass_ = 1.0, double friction_C_ = 0.0, double restitution_C_ = 1.0,
                                    bool graviityAffected_ = true, bool dragAffected_ = true,
                                    const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0,double inertia_ = 1.0);
    template <typename managerT>
    static RotatePhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &cl, 
    double mass_ = 1.0, double friction_C_ = 0.0, double restitution_C_ = 1.0, 
    bool graviityAffected_ = true, bool dragAffected_ = true ,const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0);
    static void initObj(RotatePhysicsObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &cl,
    double mass_ = 1.0, double friction_C_ = 0.0, double restitution_C_ = 1.0,
    bool graviityAffected_ = true, bool dragAffected_ = true, const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0);
    
    inline void applyForceOnCenter(const gMath::tVector &force) override{}
    inline void applyImpulseOnCenter(const gMath::tVector &impulse) override{}
    inline void applyForceAtPoint(const gMath::tVector &force, const gMath::Crdinate &point) override;
    inline void applyImpulseAtPoint(const gMath::tVector &impulse, const gMath::Crdinate &point) override;
    inline void applyImpulse_v(const gMath::tVector &impulse, const gMath::tVector &r) override;
    double getInverseInertia() const override{ return inverseInertia; }
    double getInertia() const { return Inertia; }
    inline void act() override {
        angleVelocity += angleAcceleration;
        if (dragAffected){
            angleVelocity *= 1 / (1 + mainPhysicsEngine->GLOBAL_ANGULAR_DAMPING);
        }
        mainPhysicsEngine->getCstManager()->addObjToIntegrateList(this);
    }
    void Integrate(double dt) override{
        angle += angleVelocity*dt;
    }
    void moveFix(const gMath::tVector &&fix) override{}
    void rotateFix(const gMath::Angle &&anglefix) override;
};


class LiberalPhysicsObj : virtual public MovePhysicsObj, virtual public RotatePhysicsObj{
protected:
    LiberalPhysicsObj() : MovePhysicsObj(), RotatePhysicsObj(){}
public:
    /**
     * @brief Acts on the object.
     *
     * This method is responsible for performing the actions needed by the object.
     * It calls the act() method of the MovePhysicsObj and RotatePhysicsObj classes.
     */
    inline void act() override {
        velocity += acceleration;
        if (gravityAffected){
            velocity += mainPhysicsEngine->GLOBAL_GRAVITY;
        }
        if (dragAffected){
            velocity *= 1/(1+ mainPhysicsEngine -> GLOBAL_LINEAR_DAMPING);
            angleVelocity *= 1/(1+ mainPhysicsEngine -> GLOBAL_ANGULAR_DAMPING);
        }
        mainPhysicsEngine ->getCstManager()->addObjToIntegrateList(this);
    }
    /// Indicates whether the object can be moved.
    static const bool movable = true;

    /// Indicates whether the object can be rotated.
    static const bool rotable = true;

    /// Indicates whether the object is an entity.
    static const bool isEntity = true;

    /**
     * @brief Applies a force to the object's center.
     *
     * This method applies a force to the center of the object.
     *
     * @param force The force to be applied.
     */
    inline void applyForceOnCenter(const gMath::tVector &force) override {
        MovePhysicsObj::applyForceOnCenter(force);
    }
    /**
     * @brief Applies an impulse to the object's center.
     *
     * This method applies an impulse to the center of the object.
     *
     * @param impulse The impulse to be applied.
     */
    inline void applyImpulseOnCenter(const gMath::tVector &impulse) override {
        MovePhysicsObj::applyImpulseOnCenter(impulse);
    }
    /**
     * @brief Applies a force to a point.
     *
     * This method applies a force to a point of the object.
     *
     * @param force The force to be applied.
     * @param point The point to which the force is applied.
     */
    inline void applyForceAtPoint(const gMath::tVector &force,const gMath::Crdinate &point) override {
        MovePhysicsObj::applyForceAtPoint(force,point);
        RotatePhysicsObj::applyForceAtPoint(force,point);
    }
    /**
     * @brief Applies an impulse to a point.
     *
     * This method applies an impulse to a point of the object.
     *
     * @param impulse The impulse to be applied.
     * @param point The point to which the impulse is applied.
     */
    inline void applyImpulseAtPoint(const gMath::tVector &impulse,const gMath::Crdinate &point) override {
        MovePhysicsObj::applyImpulseAtPoint(impulse,point);
        RotatePhysicsObj::applyImpulseAtPoint(impulse,point);
    }
    inline void applyImpulse_v(const gMath::tVector &impulse, const gMath::tVector &r) override {
        MovePhysicsObj::applyImpulse_v(impulse,r);
        RotatePhysicsObj::applyImpulse_v(impulse,r);
    }
    template <typename managerT>
    static LiberalPhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_ = 0.0)
    {
        return basicObjInit<LiberalPhysicsObj>(m, crd, angle_);
    }
    
    template <typename managerT>
    static LiberalPhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &cl, 
    double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_, 
    const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0, double inertia_ = 1.0,
    const gMath::tVector &initialVelocity = gMath::tVector(0.0 , 0.0), const gMath::tVector &initialAccelr = gMath::tVector(0.0 , 0.0)
    )
    {
        LiberalPhysicsObj &t = m.template acquire<LiberalPhysicsObj>();
        initObj(&t, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, angleV, angleA, inertia_, initialVelocity, initialAccelr);
        return t;
    }
    static void initObj(LiberalPhysicsObj *t, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &cl,
    double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_,
    const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0, double inertia_ = 1.0,
    const gMath::tVector &initialVelocity = gMath::tVector(0.0, 0.0), const gMath::tVector &initialAccelr = gMath::tVector(0.0, 0.0)){
        MovePhysicsObj::initObj(t, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_);
        RotatePhysicsObj::initObj(t, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, angleV, angleA, inertia_);
    }
    template <typename managerT>
    static LiberalPhysicsObj &newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &cl, 
    double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_, 
    const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0, 
    const gMath::tVector &initialVelocity = gMath::tVector (0.0, 0.0), const gMath::tVector &initialAccelr = gMath::tVector (0.0, 0.0)
    )
    {
        return newObj(m, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, angleV, angleA, caculateInertia(mass_, cl.getShape()), initialVelocity, initialAccelr);
    }
    static void initObj(LiberalPhysicsObj *t, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &cl,
    double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_,
    const gMath::Angle &angleV = 0.0, const gMath::Angle &angleA = 0.0,
    const gMath::tVector &initialVelocity = gMath::tVector(0.0, 0.0), const gMath::tVector &initialAccelr = gMath::tVector(0.0, 0.0)){  
        initObj(t, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, angleV, angleA, caculateInertia(mass_, cl.getShape()),initialVelocity, initialAccelr);
    }
    inline void Integrate(double dt);
    void moveFix(const gMath::tVector &&fix) override;
    void rotateFix(const gMath::Angle &&anglefix) override;
};



template <typename managerT>
inline RotatePhysicsObj &RotatePhysicsObj::newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &cl, double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_, const gMath::Angle &angleV, const gMath::Angle &angleA ,double inertia_)
{
        RotatePhysicsObj &t = m.template acquire<RotatePhysicsObj>();
        initObj(&t, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, angleV, angleA, inertia_);
        return t;
}
#include <vector>
double caculateInertia(double mass_, std::vector<gMath::tVector> *shape);
template <typename managerT>
inline RotatePhysicsObj &RotatePhysicsObj::newObj(managerT &m, const gMath::Crdinate &crd, gMath::Angle angle_, clsn::CollisionBox &cl, double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_, const gMath::Angle &angleV , const gMath::Angle &angleA )
{
    return newObj(m, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, caculateInertia(mass_, cl.getShape()));
}

inline void RotatePhysicsObj::initObj(RotatePhysicsObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &cl, double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_, const gMath::Angle &angleV, const gMath::Angle &angleA, double inertia_)
{
    PhysicsObj::initObj(pt, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_);
    RotateObj::initObj(pt, crd, angle_, angleV, angleA);
    pt->Inertia = inertia_;
    pt->inverseInertia = 1 / inertia_;
}

inline void RotatePhysicsObj::initObj(RotatePhysicsObj *pt, const gMath::Crdinate &crd, const gMath::Angle &angle_, clsn::CollisionBox &cl, double mass_, double friction_C_, double restitution_C_, bool graviityAffected_, bool dragAffected_, const gMath::Angle &angleV, const gMath::Angle &angleA)
{
    initObj(pt, crd, angle_, cl, mass_, friction_C_, restitution_C_, graviityAffected_, dragAffected_, angleV, angleA, caculateInertia(mass_, cl.getShape()));
}

inline void RotatePhysicsObj::applyForceAtPoint(const gMath::tVector &force, const gMath::Crdinate &point)
{
    // Calculate the torque caused by the force
    gMath::tVector displacement = (point - this->crd);
    double torque = force.cross(displacement);

    // Increase the angular acceleration
    angleAcceleration = torque * inverseInertia;
    
}

inline void RotatePhysicsObj::applyImpulseAtPoint(const gMath::tVector &impulse,const gMath::Crdinate &point)
{
    // Calculate the momentum caused by the impulse
    gMath::tVector displacement = point - this->crd;
    double angularMomentum = impulse.cross(displacement);
    // Increase the angular Velocity
    angleVelocity += angularMomentum * inverseInertia;
}

inline void RotatePhysicsObj::applyImpulse_v(const gMath::tVector &impulse, const gMath::tVector &r)
{
    double angularMomentum = impulse.cross(r);
    // Increase the angular Velocity
    angleVelocity += angularMomentum * inverseInertia;
}

inline void RotatePhysicsObj::rotateFix(const gMath::Angle &&anglefix)
{
    angle += anglefix;
}

inline void LiberalPhysicsObj::Integrate(double dt)
{
    MovePhysicsObj::Integrate(dt);
    RotatePhysicsObj::Integrate(dt);
}
inline void LiberalPhysicsObj::moveFix(const gMath::tVector &&fix)
{
    MovePhysicsObj::moveFix(std::move(fix));
}
inline void LiberalPhysicsObj::rotateFix(const gMath::Angle &&anglefix)
{
    RotatePhysicsObj::rotateFix(std::move(anglefix));
}
#endif
