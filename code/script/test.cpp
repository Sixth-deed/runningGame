
#define GRID_NEEDED
#define PRIORITY_LEVEL1
class gObj;

#define GRID_NEEDED
#define PRIORITY_LEVEL3
class ActObj;

#define GRID_NEEDED
#define PRIORITY_LEVEL5
class EntityObj;

#define GRID_NEEDED
#define PRIORITY_LEVEL2
class HelloObj;
class gObj
{
    private:
    int a;
    public:
        gObj():a(0){}
};

class EntityObj:virtual public gObj
{};

class ActObj:public gObj
{};

class HelloObj:virtual public gObj
{};

class MoveObj:virtual public ActObj
{};

class ActivateObj:virtual public ActObj, gObj
{};

class PhysicsObj:public ActivateObj, EntityObj
{};

class MyObj:virtual public ActivateObj,EntityObj
{};

class WorldObj:virtual public HelloObj,PhysicsObj
{};