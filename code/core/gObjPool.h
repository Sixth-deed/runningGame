
#ifndef GAME_gObjPool_AND_MANAGER
#define GAME_gObjPool_AND_MANAGER
#include "macros.h"
#include <vector>
#include <climits>
#include <tuple>
#include <stdexcept>
using mID = unsigned int;

//对象池，用于管理对象分配和释放
//gObjType --任意gObj类型
template <typename gObjType>
class gObjPool;

//id管理器
class idHandler{
    private:
        mID idPT;
        static idHandler* mainIdHandler;
    public:
        idHandler(): idPT(0){}
        //获取一个有效的新id
        mID getNewID(){
            if (idPT == UINT_MAX){
                throw std::runtime_error("No More ID for use!"); 
            }
            return idPT++;
        }


        //给其他模块访问mainIDHandler的引用的方式
        static idHandler& mainHandler(){
            return *mainIdHandler;
        }
};

template <typename gObjType>
class gObjPool{
    private :
        std::vector<gObjType*> pool;
    public :
        //n -- 对象池初始大小
        //调用对应模板的默认构造函数
        gObjPool(int n) : pool(n,new gObjType()){}
        gObjPool(): pool(){}
        //获取对象
        gObjType& acquire();
        //释放对象
        void release(gObjType* pt){
            //将对象指针压入内存池（可用对象）
            pool.push_back(pt);
        }
        //释放对象
        void release(gObjType& pt){
            //将对象指针压入内存池（可用对象）
            pool.push_back(&pt);
        } 
        //清空内存池
        void clear(){
            for (gObjType* pt : pool){
                delete pt;
            }
            pool.clear();
        }

        ~gObjPool(){
            clear();
        }
};
//用于管理所有类型的游戏对象（继承自gObj）的管理器
//Types是所有传入的类型模板
template <typename... Types>
class ObjectManager {
    //gpt generated
    public:
        template <typename gObjType>
            //获取对象
            gObjType* acquire() {
                return getPool<gObjType>().acquire();
            }

        template <typename gObjType>
            //释放对象
            void release(gObjType* obj) {
                getPool<gObjType>().release(obj);
            }
        //清理内存    
        void clear() {
            clearPools(std::index_sequence_for<Types...>{});
        }

    private:
        template <typename gObjType>
            gObjPool<gObjType>& getPool() {
                return std::get<gObjPool<gObjType>>(pools);
            }

        template <std::size_t... Is>
            void clearPools(std::index_sequence<Is...>) {
                (std::get<Is>(pools).clear(), ...);
            }

        std::tuple<gObjPool<Types>...> pools;
};


class gObj;
//TEMP_TYPE做占位类，编译前会修改为所有继承自gObj类的类
#define TEMP_TYPE gObj 
ObjectManager<TEMP_TYPE> MainObjectManager;

#endif
