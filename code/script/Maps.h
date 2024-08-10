#include "GameMain.h"
#include "Gmath.h"
#include "BaseObj.h"




class MapA:virtual public mGame
{
    MapA();

    mGame& initializeGame();

};



class MapC:virtual public mGame
{
    MapC();

    mGame& initializeGame();

};



class MapB:virtual public mGame
{
    MapB();

    mGame& initializeGame();

};
