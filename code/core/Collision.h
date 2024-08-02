#ifndef GAME_OBJECT_COLLISION
#define GAME_OBJECT_COLLISION
#include "Gmath.h"

#include <vector>
namespace clsn{
   using namespace  gMath;
   class CollisionBox{
    
   private :
       //left, roght, top, bottom
       //用于粗略判定的边界
       //自动生成
       axisV l,r,t,b;
       //相对于中心的向量组，规定为顺时针方向
       std::vector<tVector> vectors;
   public :
       CollisionBox(const vector<tVector>& array): vectors(array){
            l = r = array[0].x;
            t = b = array[0].y;
            for (int i = 1;i < array.size();i++){
                l=std::min(l , array[i].x);
                r=std::max(r , array[i].x);
                t=std::max(t , array[i].y);
                b=std::min(b , array[i].y);
            } 
       }
   };
}


#endif
