#ifndef GAME_OBJECT_COLLISION
#define GAME_OBJECT_COLLISION
#include "Gmath.h"
#include "gObj.h"
#include <vector>
namespace clsn{

    using namespace  gMath;
    class CollisionBox;
    class CollisionBox{
        private :
            //left, roght, top, bottom
            //用于粗略判定的边界
            //自动生成
            axisV l,r,t,b;
            //相对于中心的向量组，规定为顺时针方向的点连成的图形
            //必须保证是凸多边形
            std::vector<tVector> &vectors;
            std::vector<tVector> vectors_p;
            //所有边的法向量
            std::vector<tVector> ns;
        public :
            CollisionBox(std::vector<tVector>* array); 
            //投影范围，辅助SAT判定
            struct Projection{double low; double high;
                void addOffset(double off){
                    low += off;
                    high += off;
                }
            };

            //碰撞判定
            //如果发生碰撞，返回最小分离向量; 如果不发生碰撞，返回（0,0）
            friend tVector isIntersects(const Crdinate &crd1 ,const CollisionBox& b1,const Crdinate &crd2, const CollisionBox& b2);    
            //将整个碰撞箱投影到某条轴上
            Projection projectTo(const tVector& axis) const;
    };
    tVector isIntersects(const Crdinate &crd1 ,const CollisionBox& b1,const Crdinate &crd2, const CollisionBox& b2);    
   
}


#endif
