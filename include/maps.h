#ifndef GAME_MAPS
#define GAME_MAPS

#include "core/GameMain.h"
#include "GameObjects/RectangleObject.h"


class testmapMap : public mGame<ObjectManager<StableRectangleObj, LiberalRectangleObj>, EntityObj, ActObj, gObj>{
    private:
        testmapMap(std::initializer_list<std::size_t> initialSizes, std::initializer_list<std::tuple<int, int>> gridsInitialize, const gMath::mRectangle &rect, PhysicsEngine* const engine): mGame(initialSizes, gridsInitialize, rect, engine){}
    public :
    static testmapMap*  initializeGame();
    
};
#endif
