#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H 

#include <vector>
#include <string>
#include "lib/Gmath.h"
#include "lib/ObjPool.h"
#include "lib/idHandler.h"


namespace mtype{
    enum gInstanceTypes{
        gInstance,
		StableRectangleObj,
		LiberalRectangleObj
	};
};

#define DECLARE_INSTANCE_TYPE(TypeName)                \
    protected:                                         \
    static constexpr mtype::gInstanceTypes Type = mtype::gInstanceTypes::TypeName; \
    public :                                           \
    virtual constexpr mtype::gInstanceTypes getType() const { return Type ; } \
    private: 



#define GRID_NEEDED
#define PRIORITY_LEVEL0
//游戏对象基类
//规定了游戏对象的一些基本属性
class gObj {
    DECLARE_INSTANCE_TYPE(gInstance)
protected:
    static idHandler *mIdHandler;
    //友元类
    template <typename U>
    friend class ObjPool;

    //对象坐标
    gMath::Crdinate crd;
    //对象id
    mID id;
    gMath::Angle angle;
    gObj(gMath::axisV x, gMath::axisV y, gMath::Angle angle_ = 0.0):crd(x,y), angle(angle_){}
    gObj(const gMath::Crdinate& cr,  gMath::Angle angle_ = 0.0):crd(cr), angle(angle_){}
    gObj(): crd(0,0),id(mIdHandler->getNewID()),angle(0.0) {}
    //用来记录是否被遍历与更新，除了ActObj，其他类固定为true
    bool isSleeping =true;
public:
    virtual std::string log()const {
        std::string flagstring;
        if (!flags.empty()){
            flagstring = "flags = [";
            for (auto f : flags){
                flagstring += f + ",";
            }
            flagstring.pop_back();
            flagstring += "]\n";
        }
        return "{\n\tid = " + std::to_string(id) + ",\n\t" +
               "crd = " + crd.log() + ",\n\t" +
               "angle = " + angle.log() + ",\n\t" +
               "sleeping = " + std::to_string(isSleeping) + ",\n\t" + 
               flagstring +
               "}\n";
    }
    //用于将关于这个对象的有用的额外信息传往前端
    std::vector<std::string> flags;
    //获得一个对象
/*    template <typename managerT>
    static gObj& newObj(managerT &m,const gMath::Crdinate& crd, const gMath::Angle &angle_){
        gObj* pt = m.template acquire<gObj>();
        initObj(pt,crd,angle_);
        return *pt;
    } */
    //初始化对象,子类应该重载
    static void initObj(gObj* pt, const gMath::Crdinate& crd , const gMath::Angle &angle_){
        pt->crd = crd;
        pt->angle = angle_;
    }
    const gMath::Crdinate& getCrd() const {
        return crd;
    }
    gMath::axisV get_x() const{
        return crd.get_x();
    }
    gMath::axisV get_y() const {
        return crd.get_y();
    }
    mID getID() const { return id; }
    void setCoordinate(gMath::Crdinate crd_){
        crd=crd_;
    }
    gMath::Angle getAngle() const {
        return angle;
    }
    void setAngle(const gMath::Angle& angle_) {
        angle = angle_;

    }
    static const bool movable = false;
    static const bool rotatable = false;
    static const bool is_an_Entity = false;
    virtual bool isMovable() const {return movable;}
    virtual bool isRotatable()  const {return rotatable;}
    virtual bool isEntity() const {return is_an_Entity;}
    bool isSleep(){return isSleeping;}
    void setSleep(){isSleeping = true;}
    
    static void setIDhandler(idHandler*const pt) {mIdHandler = pt;}

    /*
    //此处的SendPackType类型需要实现，Container选择合适的容器即可
    SendPackType send() {
        //to be finished
        //应该完成当前对象坐标，对象id，对象角度，对象类型以及flags的打包
        
        return SendPackType(...);      

        //当前能想到的需要做的优化：

        //1.使用右值传递SendPackType, 避免拷贝
        //2.这里获取对象类型尽量不用c++运行时动态类型，保证能够编译时确定
        
        
    }

    //另一种可选的发送方式
    void send(SendPackContainer& container){
        container.push((SendPackType)...); 
    }*/
    virtual ~gObj(){}
};
template <typename managerT, typename gObjType>
inline gObjType& basicObjInit(managerT& m,const gMath::Crdinate& crd = gMath::Crdinate(0, 0 ), const gMath::Angle& angle_ = 0.0){
    gObjType& gobj = m.template acquire<gObjType>();
    gobj.setCoordinate(crd);
    gobj.angle = angle_;
    return gobj;
}
#endif 