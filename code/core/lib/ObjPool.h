
#ifndef GAME_ObjPool_AND_MANAGER
#define GAME_ObjPool_AND_MANAGER
#include <vector>
#include <climits>
#include <tuple>
#include <stdexcept>

//对象池，用于管理对象分配和释放
//gObjType --任意gObj类型
template <typename gObjType>
class ObjPool;

/**
 * @class ObjPool
 * @brief A template class for managing a pool of game objects.
 * 
 * This class provides a way to manage a pool of game objects, allowing for efficient
 * creation and destruction of objects. It uses a vector to store the objects and
 * provides methods for acquiring and releasing objects.
 * 
 * @tparam gObjType The type of game object to be managed.
 */
template <typename gObjType>
class ObjPool{
    private :
        std::vector<gObjType*> pool;
    public :
        //n -- 对象池初始大小
        //调用对应模板的默认构造函数
        ObjPool(int n) : pool(n,new gObjType()){}
        ObjPool(): pool(){}
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

        ~ObjPool(){
            clear();
        }
};
//用于管理所有类型的对象的管理器
//Types是所有传入的类型模板
template <typename... Types>
/**
 * @class ObjectManager
 * @brief A manager class for managing all types of game objects that inherit from gObj.
 * It provides methods for acquiring and releasing objects.
 * 
 * @tparam Types The types of game objects to be managed.
 */
class ObjectManager {
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
             return std::tuple<ObjPool<Types>...>(ObjPool<Types>(initialSizes[Is])...);
        }
        /**
         * Creates a tuple of ObjPool instances for the given types, 
         * initializing each pool with the corresponding size from the initialSizes array.
         *
         * @param std::index_sequence<Is...> an index sequence representing the types
         * @param const std::size_t* initialSizes an array of initial sizes for the pools
         *
         * @return a tuple of ObjPool instances
         */
        template <std::size_t... Is> 
        auto createPools(std::index_sequence<Is...>, const std::size_t* initialSizes) {
             return std::tuple<ObjPool<Types>...>(ObjPool<Types>(initialSizes[Is])...);
        }

        template <typename gObjType>
            ObjPool<gObjType>& getPool() {
                return std::get<ObjPool<gObjType>>(pools);
            }

        /**
         * Clears all object pools managed by this ObjectManager instance.
         *
         * @tparam Is  a parameter pack of indices corresponding to the object pools to clear
         *
         * @param std::index_sequence<Is...>  an index sequence parameter used to unpack the parameter pack
         *
         * @return void
         */
        template <std::size_t... Is>
            void clearPools(std::index_sequence<Is...>) {
                (std::get<Is>(pools).clear(), ...);
            }

        std::tuple<ObjPool<Types>...> pools;
};



#endif
