#include "Collision.h"
#include <cfloat>
#include <cmath>
using namespace clsn;
CollisionBox::Projection CollisionBox::projectTo(const tVector& axis) const{
    double max, min =max= vectors[0].dot(axis);
    for (int i = 1; i < vectors.size();i++){
        double projection_i = vectors[i].dot(axis);
        min = std::min(min , projection_i);
        max = std::max(max , projection_i);
    }
    return {min,max};
}


tVector clsn::isIntersects(const Crdinate &crd1 ,const CollisionBox& b1,const Crdinate &crd2, const CollisionBox& b2){
    //由对象1的中心点指向对象2的中心点的向量
    tVector offset12 = crd2 - crd1;
    //先做aabb粗检测
    if ( offset12.y + b2.b - b1.t >= 0 
        || offset12.y + b2.t - b1.b <=0
        || offset12.x + b2.l -b1.r >=0
        || offset12.x + b2.r - b1.l <=0){
        return tVector(0.0,0.0);
    }
    //如果粗检测相交，进行SAT检测
    //记录最小位移向量
    tVector minV(0.0,0.0);
    double mMin=DBL_MAX;
    for (const auto& axis : b1.ns){
        auto p1 = b1.projectTo(axis);
        auto p2 = b2.projectTo(axis);
        p2.addOffset(offset12.dot(axis));
        if (p1.high <p2.low || p2.high <p1.low){
            return tVector(0.0 ,0.0);
        }
        double temp=(p2.high > p1. high ? p1.high - p2.low : p2.high - p1.low);
        if (mMin >temp){
            mMin = temp;
            minV = axis;
        }
    }
    for (const auto& axis : b2.ns){
        auto p1 = b1.projectTo(axis);
        auto p2 = b2.projectTo(axis);
        p2.addOffset(offset12.dot(axis));
        if (p1.high <p2.low || p2.high <p1.low){
            return tVector(0.0 ,0.0);
        }
        double temp=(p2.high > p1. high ? p1.high - p2.low : p2.high - p1.low);
        if (mMin >temp){
            mMin = temp;
            minV = axis;
        }
    }
    return minV;
   
}

CollisionBox::CollisionBox( std::vector<tVector>* array): vectors(*array) ,vectors_p(array),ns(array.size()){
                double tl,tr,tt,tb;
                tl = tr = array[0].x;
                tt = tb = array[0].y;
                ns[0]=array[0].normal_and_unify();
                for (int i = 1;i < array.size();i++){
                    tl=std::min(tl , array[i].x);
                    tr=std::max(tr , array[i].x);
                    tt=std::max(tt , array[i].y);
                    tb=std::min(tb , array[i].y);
                    ns[i]= array[i].normal_and_unify();
                } 
                l = static_cast<axisV>(tl);
                r = static_cast<axisV>(tr);
                t = static_cast<axisV>(tt);
                b = static_cast<axisV>(tb);
            }
