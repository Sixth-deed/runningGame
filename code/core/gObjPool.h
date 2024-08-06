
#ifndef GAME_gObjPool_AND_MANAGER
#define GAME_gObjPool_AND_MANAGER
#include "macros.h"
#include "idHandler.h"
#include <vector>
#include <climits>
#include <tuple>
#include <stdexcept>

//对象池，用于管理对象分配和释放
//gObjType --任意gObj类型
template <typename gObjType>
class gObjPool;


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
        
        // 构造函数接受一个长度和模板长度相同的数组，用于初始化不同类型的对象池
        explicit ObjectManager(std::size_t initialSizes[sizeof...(Types)] )
        : pools(createPools(std::make_index_sequence<sizeof...(Types)>(), initialSizes)) {}
        // 构造函数接受一个 initializer_list，用于初始化不同类型的对象池
        explicit ObjectManager(std::initializer_list<std::size_t> initialSizes )
        : pools(createPools(std::make_index_sequence<sizeof...(Types)>(), initialSizes.begin())) {}
          
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
        // 创建 ObjectPool 实例的辅助函数
        template <std::size_t... Is>
        auto createPools(std::index_sequence<Is...>, std::size_t initialSizes[sizeof...(Types)]) {
             return std::tuple<gObjPool<Types>...>(gObjPool<Types>(initialSizes[Is])...);
        }
        template <std::size_t... Is> 
        auto createPools(std::index_sequence<Is...>, const std::size_t* initialSizes) {
             return std::tuple<gObjPool<Types>...>(gObjPool<Types>(initialSizes[Is])...);
        }

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



#endif
