#ifndef QUADTREE_GRID_AND_REFERENCE_MANAGER
#define QUADTREE_GRID_AND_REFERENCE_MANAGER

//采用四叉树管理整个地图的对象引用分配
#include "Gmath.h"
#include "gObjPool.h"
#include <unordered_map>
#include <vector>
//方格类，用于四叉树优化
template <typename gObjType>
class Grid;

//用于管理某类型方格系统的方格与引用
template <typename gObjType>
class GridManager;

template <typename gObjType>
class Grid{
    private :
        //left, right, top, bottom
        gMath::axisV l , r , t ,b;
        //left top, right top, left top, right topg
        Grid<gObjType>* lt, *rt, *lb, *rb;
        //是否已经被分割
        bool divided=false;
        //当前方格的id
        mID gridID;
        //id管理器
        static idHandler gIDhdr;
        
        //获得一个引用的code, 用于传给Manager
        using ull=unsigned long long;
        ull rfCode(mID objid) {return (static_cast<ull>(gridID)<<32)+ objid;}
        //指向manager的指针
        GridManager<gObjType>* mManager;
        
        //持有的对象数字达到这个数就分割
        const short threshold ;
        //分割用的函数
        void divideSlef();
    public :
        Grid(): l(0),r(0),t(0),b(0),threshold(10){}
        Grid(gMath::axisV l,gMath::axisV r,gMath::axisV t,gMath::axisV b
            , const std::vector<gObjType*>& v
            , short thrs = 10);
        //通过id获得对象的引用
        gObjType& getObj(mID objID) const;
        void insert(gObjType* pObj);
        void erase(mID objID);
        void erase(gObjType* pObj);
};
template <typename gObjType>
class GridManager{
private:
    using ull=unsigned long long;
    std::unordered_map<ull,gObj*> referencePool;

    std::vector<Grid<gObjType>> gridPool;
public:
    GridManager(int rfPoolbuckets, int gridPoolInitialVolume): referencePool(rfPoolbuckets), gridPool(gridPoolInitialVolume){}
    gObj* get(ull code) const { return referencePool.at(code);}  
    void insert(ull code, gObj* pt) { referencePool.emplace(code , pt); }
    void erase(ull code) { referencePool.erase(code); }
    void clear() {referencePool.clear();}
};
#endif
