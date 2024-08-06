#ifndef GAME_MAIN
#define GAME_MAIN
#include "Gmath.h"
#include "gObj.h"
#include "gObjPool.h"
#include "BaseObj.h"
#include "QuadTree.h"
#include <unordered_map>
template <typename ManagerT,typename... GridManagerTypes>
class mGame{
private:
    std::unordered_map<mID,gObj*> env;
    ManagerT mainObjManager;
    std::tuple<GridManagerTypes...>& GridManagers;
    const std::tuple<Grid<GridManagerTypes>* ...>& rootGrids;
    template <typename gObjType, typename... Args>
    gObjType& newObj(Args ...args){
        return gObjType::newObj(mainObjManager,args...);
    }
public:

    mGame(std::initializer_list<std::size_t> initialSizes, std::initializer_list<std::tuple<int,int> > gridsInitialize, const gMath::mRectangele& rect);
    ~mGame();
    static  mGame& initializeGame();    
       //example
       //假设初始地图上有moveObj2个，EntityObj1个，ActOBJ一个
       //坐标给出
       //假设源地图文件为json,内容为
       //{
       //   "boundary" : {
       //       "l" :   -1000
       //       "r" :   1000
       //       "t" :   500
       //       "b" :   -300
       //   }
       //   "content" : "gameMap"
       //   "gObjs" : {
       //       "moveObj" : {
       //           "1" : {
       //               x: 
       //               y: 
       //               ...
       //           }
       //           "2" : ...
       //           ...
       //       }
       //       "entityObj" :{ ... } 
       //       ...
       //   }
       //}
       //
       //上面的json文件存放的地图应该对应这样的初始化函数 
    static mGame& exampleInitialize()   
    {
          auto& theGame = mGame<ObjectManager<MoveObj,EntityObj,ActObj>, gObj,ActObj,EntityObj>
              ({2, 1, 1} , { {5, 1}, {3, 1} ,{1, 1} }, gMath::mRectangele(-1000,1000,500,-300));
          auto gObjRootGrid = std::get<Grid<gObj>*>(theGame.rootGrids);
          auto ActRootGrid = std::get<Grid<ActObj>*>(theGame.rootGrids);
          auto EntityRootGrid = std::get<Grid<EntityObj>*>(theGame.rootGrids);
          //初始化第一个对象
          auto& obj = newObj<MoveObj>(gMath::Crdinate{2,4});
          gObjRootGrid->insert(& obj);
          ActRootGrid->insert(&obj); 
          //初始化后续对象
          //...
    }
    
    template <std::size_t... Is> 
    std::tuple<GridManager<GridManagerTypes>...>& createGridManagers(std::index_sequence<Is...>, const std::tuple<int,int>* initialSizes) {
        
         return std::tuple<GridManager<GridManagerTypes>...>(GridManager<GridManagerTypes>(initialSizes[Is])...);
    }

    template <std::size_t... Is> 
    std::tuple<GridManager<GridManagerTypes>...>& createRootGrids   (std::index_sequence<Is...>, const gMath::mRectangele& rect         ) {
        
         return std::tuple<Grid<GridManagerTypes>* ...>(new Grid<GridManagerTypes>(rect)...);
    }
};

#endif
