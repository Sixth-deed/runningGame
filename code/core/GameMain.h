#ifndef GAME_MAIN
#define GAME_MAIN
#include "Gmath.h"
#include "gObj.h"
#include "gObjPool.h"
#include "BaseObj.h"
#include <unordered_map>
class mGame{
private:
    std::unordered_map<mID,gObj*> env;
public:
    mGame& initializeGame(){
       //example
       //假设初始地图上有moveObj2个，EntityObj1个，ActOBJ一个
       //坐标给出
       //假设源地图文件为json,内容为
       //{
       //   "content" : "gameMap"
       //   "gObjs" : {
       //       "moveObj" : {
       //           "1" : {
       //               x: ...
       //               y: ...
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
       auto& mainObjManager=*(new ObjectManager< MoveObj, EntityObj,ActObj>( {2 ,1,  1} ));
       MoveObj::newObj(gMath::Crdinate{1,2} ,mainObjManager );
       //...

    }
};

#endif
