#ifndef GAME_OBJECT_COLLISION
#define GAME_OBJECT_COLLISION
#include "Gmath.h"
#include "gObj.h"
#include <vector>
#include <algorithm>
#include <thread>

#include "lib/mOptional.h"


namespace clsn
{

    using namespace gMath;
    //碰撞箱类
    class CollisionBox;
    //用于记录碰撞的局部信息，包括两个碰撞体各自的接触点和分离法向量和切向量（规格化的）
    struct CollisionLocal;

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
            const tVector toLowPoint;
             double high;
            const tVector toHighPoint;
            Projection() = default;
            Projection(const double l, const tVector&& tlp, const double h, const tVector&& thp) : low(l), toLowPoint(tlp), high(h), toHighPoint(thp) {}
            //只启用移动语义
            Projection(Projection&& p ) = default;
            Projection(const Projection& p ) = delete;
            Projection& operator=(Projection&& p ) = default;
            Projection& operator=(const Projection& p ) = delete;
            void addOffset(double off)
            {
                low += off;
                high += off;
            }
            
        };


        friend mOptional<CollisionLocal> isReallyIntersects(const Crdinate &crd1, CollisionBox &b1, const Angle &angle1, const Crdinate &crd2, CollisionBox &b2, const Angle &angle2);
        // 将整个碰撞箱投影到某条轴上
        // 需要保证axis是单位向量
        Projection projectTo(const tVector &axis) const;
        // 整体旋转碰撞箱
        void RotateTo(const Angle &angle);
        //仅当碰撞箱是圆有用的辅助函数
        double radius() const {
            return vectors[0].x;
        }
        // 粗检测
        friend inline bool isOuterIntersect(const Crdinate &crd1, const CollisionBox &b1, const Crdinate &crd2, const CollisionBox &b2);
        std::vector<tVector>* getShape(){
            return vectors_p;
        }
    };
    
    mOptional<CollisionLocal> isReallyIntersects(const Crdinate &crd1, CollisionBox &b1, const Angle &angle1, const Crdinate &crd2, CollisionBox &b2, const Angle &angle2);
    inline bool clsn::isOuterIntersect(const Crdinate &crd1, const CollisionBox &b1, const Crdinate &crd2, const CollisionBox &b2)
    {
        // 由对象1的中心点指向对象2的中心点的向量
        tVector offset12 = crd2 - crd1;
        return offset12.y + b2.b - b1.t < 0 && offset12.y + b2.t - b1.b > 0 && offset12.x + b2.l - b1.r < 0 && offset12.x + b2.r - b1.l > 0;
    }
    
    /**
     * @brief 碰撞局部信息，包括两个碰撞体各自的接触点和分离法向量和切向量（单位化过的）
     * 
     * @var toBody1_cp 相对实体1重心指向碰撞点的向量
     * @var toBody2_cp 相对实体2重心指向碰撞点的向量
     * @var normal     碰撞法向量
     * @var tangent    碰撞切向量
     */
    struct CollisionLocal
    {
        //相对实体1重心指向碰撞点的向量
        gMath::tVector toBody1_cp;
        //相对实体2重心指向碰撞点的向量
        gMath::tVector toBody2_cp;
        gMath::tVector normal;
        gMath::tVector tangent;
        CollisionLocal(const gMath::tVector &toBody1_cp_, const gMath::tVector &toBody2_cp_, const gMath::tVector &normal_, const gMath::tVector &tangent_) : toBody1_cp(toBody1_cp_), toBody2_cp(toBody2_cp_), normal(normal_), tangent(tangent_) {}
        
        //只允许移动
        CollisionLocal(const CollisionLocal &cl) = delete;
        CollisionLocal(CollisionLocal &&cl) = default;
        CollisionLocal& operator=(const CollisionLocal &cl) = delete;
        CollisionLocal& operator=(CollisionLocal &&cl) = default;
    };
}

#endif
