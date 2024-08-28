#include "core/GameMain.h"
#include "GameObjects/RectangleObject.h"
#include "maps.h"
#include "mlogger/logger.h"
template class Grid<gObj>;
template class Grid<EntityObj>;
template class Grid<ActObj>;

template class ObjectManager<StableRectangleObj, LiberalRectangleObj>;
template class ObjPool<StableRectangleObj>;
template class ObjPool<LiberalRectangleObj>;

testmapMap *testmapMap::initializeGame()
{
    gObj::setIDhandler(new idHandler());
    PhysicsEngine *pEngine = new PhysicsEngine();
    PhysicsObj::setPhysicsEngine(pEngine);
    Constraint::setCstManager(pEngine->getCstManager());

    // 让GameLoop转起来的辅助对象
    ActiveRectangle *Trect = new ActiveRectangle(-100, 100, 200, -200);

    auto theGame = new testmapMap({1, 2}, {{3, 1}, {2, 1}, {3, 1}}, gMath::mRectangle(-1000, 1000, 500, -500), pEngine);
    auto gObjRootGrid = std::get<Grid<gObj> *>(theGame->rootGrids);
    auto ActRootGrid = std::get<Grid<ActObj> *>(theGame->rootGrids);
    auto EntityRootGrid = std::get<Grid<EntityObj> *>(theGame->rootGrids);
    
    
    
    {
        auto &obj = theGame->newObj<StableRectangleObj>(gMath::Crdinate(0, -100), gMath::Angle(0), gMath::tVector(-100.0, 10.0));
        gObjRootGrid->insert(&obj);
        EntityRootGrid->insert(&obj);
    }
    {
        auto &obj = theGame->newObj<LiberalRectangleObj>(gMath::Crdinate(-10, -80), gMath::Angle(0), gMath::tVector(-10.0, 10.0), 1.0, 0.0, 1.0, true, true);
        gObjRootGrid->insert(&obj);
        ActRootGrid->insert(&obj);
        EntityRootGrid->insert(&obj);
    }

    {
        auto &obj = theGame->newObj<LiberalRectangleObj>(gMath::Crdinate(-5, -50), gMath::Angle(30), gMath::tVector(-10.0, 10.0), 1.0, 0.0, 1.0, true, true);
        gObjRootGrid->insert(&obj);
        ActRootGrid->insert(&obj);
        EntityRootGrid->insert(&obj);
    }
    
    
    
    return theGame;

    
}
