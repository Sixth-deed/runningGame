#ifndef GAME_MATH_HELPER
#define GAME_MATH_HELPER

namespace gMath{

    using axisV = unsigned int;
    //二维向量
    class tVector{
        private:
            axisV x,y;
        public:
            tVector(axisV a, axisV b): x(a),y(b){}
            axisV dot(const tVector& v) const {
                return x*v.x +y* v.y;
            }
            tVector operator-(const tVector &v) const{
                return tVector(x-v.x, y-v.y);
            }
            //正交
            tVector normal(){
                return tVector(-y,x);
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
    };


}
#endif
