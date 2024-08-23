#include "maps.h"
int main(){
    mGameVirtualBase* game = testmapMap::initializeGame();
    gameLoopParam param;
    game -> GameLoop(param);
    return 0;
}
