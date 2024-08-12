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
            tVector& operator+=(const tVector& r){
                x += r.x;
                y += r.y;
                return *this;
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
            Crdinate& operator+=(const tVector& v){
                x += static_cast<axisV>(v.x);
                y += static_cast<axisV>(v.y);
                return *this;
            }
    };
    class mRectangle{
    protected:
        axisV l,r,t,b;
    public:
        mRectangle(axisV left, axisV right, axisV top, axisV bottom):l(left),r(right),t(top),b(bottom){}
        bool Inside(Crdinate crd){
            return crd.get_x() <= r && crd.get_y() <=t && crd.get_x() >= l && crd.get_y() >= b;
        }
        bool intersects(const mRectangle& rec){
            return l <= rec.r && r >= rec.l && t >= rec.b && b <= rec.t;
        }
    };
   class Angle {
public:
    // 构造函数
    Angle(double angleInDegrees = 0.0) : degrees(angleInDegrees) {
        normalize();
    }

    // 获取以度数表示的角度
    double getDegrees() const {
        return degrees;
    }

    // 设置以度数表示的角度
    void setDegrees(double angleInDegrees) {
        degrees = angleInDegrees;
        normalize();
    }

    // 获取以弧度表示的角度
    double getRadians() const {
        return degreesToRadians(degrees);
    }

    // 设置以弧度表示的角度
    void setRadians(double angleInRadians) {
        degrees = radiansToDegrees(angleInRadians);
        normalize();
    }

    // 角度加法
    Angle operator+(const Angle& other) const {
        return Angle(degrees + other.degrees);
    }

    // 角度减法
    Angle operator-(const Angle& other) const {
        return Angle(degrees - other.degrees);
    }
    Angle& operator+=(const Angle& other) {
        degrees += other.degrees;
        return *this;
    }

private:
    double degrees;

    // 将角度规范化到0到360度之间
    void normalize() {
        degrees = fmod(degrees, 360.0);
        if (degrees < 0) {
            degrees += 360.0;
        }
    }

    // 辅助函数：度数转弧度
    static double degreesToRadians(double degrees) {
        return degrees * M_PI / 180.0;
    }

    // 辅助函数：弧度转度数
    static double radiansToDegrees(double radians) {
        return radians * 180.0 / M_PI;
    }
};
}
#endif
