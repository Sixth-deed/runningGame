#include "maps.h"
int main(){
    mGameVirtualBase* game = testmapMap::initializeGame();
    game -> GameLoop();
    return 0;
}
