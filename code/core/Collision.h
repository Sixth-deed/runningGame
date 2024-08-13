#ifndef GAME_OBJECT_COLLISION
#define GAME_OBJECT_COLLISION
#include "Gmath.h"
#include "gObj.h"
#include <vector>
#include <algorithm>
#include <thread>
namespace clsn
{

    using namespace gMath;
    class CollisionBox;
    class CollisionBox
    {
    private:
        // left, roght, top, bottom
        // 用于粗略判定的边界
        // 自动生成
        axisV l, r, t, b;

        // 相对于中心的向量组，规定为顺时针方向的点连成的图形
        // 必须保证是凸多边形
        // 当vectors容量为1时，则标志这是一个球形的碰撞箱，规定传入的向量在x轴正方向的投影为半径长
        // 当vectors容量为2时，标志这是一个线段碰撞箱，其中只有第一个向量有效，线段为由中心出发的向量和反向延伸的向量合成的
        std::vector<tVector> &vectors;
        std::vector<tVector> *vectors_p;
        // 所有边的法向量
        std::vector<tVector> *ns;

        // 当前碰撞箱向量对应的角度
        Angle nAngle;
        //是否会发生旋转
        bool rotatable = false;
    public:
        // array传入的是相对于中心的向量组，通常规定为顺时针方向的点连成的多边形
        // 必须保证是凸多边形
        // 你不应该使用超过64边的形状，这会导致问题
        // 当vectors容量为1时，则标志这是一个球形的碰撞箱，规定传入的向量在x轴正方向的投影为半径长
        // 当vectors容量为2时，标志这是一个线段碰撞箱，其中只有第一个向量有效，线段为由中心出发的向量和反向延伸的向量合成的
        // anlge -- 初始旋转角度
        CollisionBox(std::vector<tVector> *array, bool rotatable_ = false,const Angle &angle = 0.0);
        // 投影范围，辅助SAT判定
        struct Projection
        {
            double low;
            double high;
            void addOffset(double off)
            {
                low += off;
                high += off;
            }
        };

        /**
 * Checks if two objects are really intersecting, taking into account their collision boxes and angles.
 *
 * @param crd1 The coordinate of the first object.
 * @param b1 The collision box of the first object.
 * @param angle1 The angle of the first object.
 * @param crd2 The coordinate of the second object.
 * @param b2 The collision box of the second object.
 * @param angle2 The angle of the second object.
 *
 * @return A vector representing the minimum translation to separate the two objects, or a zero vector if they are not intersecting.
 */
        friend tVector isReallyIntersects(const Crdinate &crd1, CollisionBox &b1, const Angle &angle1, const Crdinate &crd2, CollisionBox &b2, const Angle &angle2);
        // 将整个碰撞箱投影到某条轴上
        Projection projectTo(const tVector &axis) const;
        // 整体旋转碰撞箱
        void RotateTo(const Angle &angle);
        //仅当碰撞箱是圆有用的辅助函数
        double radius() const {
            return vectors[0].x;
        }
        // 粗检测
        inline friend bool isOuterIntersect(const Crdinate &crd1, const CollisionBox &b1, const Crdinate &crd2, const CollisionBox &b2);
        std::vector<tVector>* getShape(){
            return vectors_p;
        }
    };
    
    tVector isReallyIntersects(const Crdinate &crd1, CollisionBox &b1, const Angle &angle1, const Crdinate &crd2, CollisionBox &b2, const Angle &angle2);
    inline bool clsn::isOuterIntersect(const Crdinate &crd1, const CollisionBox &b1, const Crdinate &crd2, const CollisionBox &b2)
    {
        // 由对象1的中心点指向对象2的中心点的向量
        tVector offset12 = crd2 - crd1;
        return offset12.y + b2.b - b1.t < 0 && offset12.y + b2.t - b1.b > 0 && offset12.x + b2.l - b1.r < 0 && offset12.x + b2.r - b1.l > 0;
    }
}

#endif
