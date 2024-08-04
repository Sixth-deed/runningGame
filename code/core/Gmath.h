#ifndef GAME_MATH_HELPER
#define GAME_MATH_HELPER
#include <cmath>
namespace gMath{

    using axisV = unsigned int;
    //二维向量
    class tVector{
        public:
            double x,y;
            tVector(double a, double b): x(a), y(b){}
            tVector(axisV a, axisV b): x(a),y(b){}
            tVector(const tVector& r): x(r.x), y(r.y){}
            double dot(const tVector& v) const {
                return x*v.x +y* v.y;
            }
            tVector operator-(const tVector &v) const{
                return tVector(x-v.x, y-v.y);
            }
            //正交
            tVector normal() const {
                return tVector(-y,x);
            }
            //返回一个正交的单位向量
            tVector normal_and_unify() const{
                return normal()/std::sqrt(sLen());
            }
            tVector operator=(const tVector & r){
                x=r.x,y=r.y;
                return *this;
            }
            //模长的平方
            double  sLen() const{
                return x*x + y*y;
            }
            tVector operator/(double a) const{
                return tVector(x/a,y/a);
            }
    };
    //坐标类
    class Crdinate {
        private :
            axisV x;
            axisV y;
        public :
            Crdinate(axisV x_, axisV y_): x(x_),y(y_){}

            void  set_x(axisV x_){
                this->x = x_;
            }
            void  set_y(axisV y_){
                this->y = y_;
            }
            axisV get_x(){
                return x;
            }
            axisV get_y(){
                return y;
            }
            Crdinate operator=(const Crdinate& crd){
                x = crd.x;
                y = crd.y;
                return *this;
            }
            tVector operator+(const tVector& v) const {
                return tVector(x+v.x , y+ v.y);
            }
            tVector operator-(const Crdinate& r) const{
                return tVector(x-r.x , y-r.y);
            }
    };


}
#endif
