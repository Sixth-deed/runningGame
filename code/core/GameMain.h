#ifndef GAME_MAIN
#define GAME_MAIN
#include "Gmath.h"
#include "gObj.h"
#include "gObjPool.h"
#include "BaseObj.h"
#include "QuadTree.h"
#include <unordered_map>
#include <utility>

/*
游戏实例基类

模板参数
ManagerT - 采用的对象管理器的类型, 需要包含所有的可能用到的对象类型
GridManagerTypes - 需要套用Grid模块管理的对象的类型, 一定要包括gObj, EntityObj, ActObj
*/
template <typename ManagerT,typename... GridManagingGameObjTypes>
class mGame{
protected:
    std::unordered_map<mID,gObj*> env;
    
    ManagerT mainObjManager;
    
    std::tuple<GridManager<GridManagingGameObjTypes>...>& GridManagers;
    
    const std::tuple<Grid<GridManagingGameObjTypes>* ...>& rootGrids;
    
    std::tuple<std::vector<Grid<GridManagingGameObjTypes>*>* ...> activeGridsTuple;
    
    
    void deleteActiveGridsRef();

    template <typename gObjType, typename... Args>
    gObjType& newObj(Args ...args){
        return gObjType::newObj(mainObjManager,args...);
    }
    template <typename gObjType>
    Grid<gObjType>* rootGrid(){
        return std::get<Grid<gObjType>*> (rootGrids);
    }
    template <typename gObjType>
    std::vector<Grid<gObjType>>* activeGrids(){
        return std::get<gObjType>(activeGridsTuple);
    }
    void updateActiveGrids();
    void initializeActiveGrids();
    mGame(std::initializer_list<std::size_t> initialSizes, std::initializer_list<std::tuple<int,int> > gridsInitialize, const gMath::mRectangele& rect);
public:
    ~mGame();
    //初始化游戏
    static  mGame& initializeGame();  
    
      
       //example
       //假设初始地图上有moveObj2个，EntityObj1个，ActOBJ一个
       //坐标给出
       //假设源地图文件为map.json,内容为
       //{
       //   "boundary" : {
       //       "l" :   -1000
       //       "r" :   1000
       //       "t" :   500
       //       "b" :   -300
       //   }
       //   "gObjs" : {
       //       "moveObj" : {
       //           "1" : {
       //               x: 2
       //               y: 4

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
          auto& theGame = mGame< ObjectManager<MoveObj,EntityObj,ActObj>    , gObj,ActObj,EntityObj>
              ({2, 1, 1} , { {4, 1}, {3, 1} ,{1, 1} }, gMath::mRectangele(-1000,1000,500,-300));
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
    std::tuple<GridManager<GridManagingGameObjTypes>...>& createGridManagers(std::index_sequence<Is...>, const std::tuple<int,int>* initialSizes) {
        
         return std::tuple<GridManager<GridManagingGameObjTypes>...>(GridManager<GridManagingGameObjTypes>(initialSizes[Is])...);
    }

    template <std::size_t... Is> 
    std::tuple<Grid<GridManagingGameObjTypes>*...>& createRootGrids   (std::index_sequence<Is...>, const gMath::mRectangele& rect) {
        
         return std::tuple<Grid<GridManagingGameObjTypes>* ...>(new Grid<GridManagingGameObjTypes>(rect)...);
    }
    void GameLoop();
};

#endif
