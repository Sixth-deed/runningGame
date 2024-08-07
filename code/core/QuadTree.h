#ifndef QUADTREE_GRID_AND_REFERENCE_MANAGER
#define QUADTREE_GRID_AND_REFERENCE_MANAGER

//采用四叉树管理整个地图的对象引用分配
#include "Gmath.h"
#include "idHandler.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
//方格类，用于四叉树优化
template <typename gObjType>
class Grid;

//用于管理某类型方格系统的方格与引用
template <typename gObjType>
class GridManager;

//用于区分活跃部分和不活跃部分
class ActiveRectangle;
class gObj;

template <typename gObjType>
class Grid{
    static_assert(std::is_base_of<gObj, gObjType>::value, "the template type parameter must be a subclass of gObj!");

    friend GridManager<gObjType>;
    private :
        //left, right, top, bottom
        gMath::mRectangele rect;
        //left top, right top, left top, right top
        Grid<gObjType>* lt, *rt, *lb, *rb;
        //如果分割自某个方格持有其指针
        Grid<gObjType>* parentGrid;
        //持有根指针
        Grid<gObjType>* root;

        //是否已经被分割
        bool divided=false;
        //当前方格的id
        mID gridID;
        //id管理器
        static idHandler gIDhdr;

        
        //指向manager的指针
        static GridManager<gObjType>* mManager;
        
        //持有的对象数字达到这个数就分割
        static const unsigned int threshold = 10;
        //分割用的函数
        void divideSelf();
        //合并用函数
        void collectSelf();
        //辅助函数, 把inputGrid的引用全部添加到this的引用池中
        void collectFrom(const Grid<gObjType>*);
        //默认构造函数, GridManager 构造新对象时调用
        Grid(): rect(0,0,0,0), gridID(gIDhdr.getNewID()) {}
        Grid(gMath::axisV l,gMath::axisV r,gMath::axisV t,gMath::axisV b
            , const std::vector<gObjType*>& v);
        template <typename... Args>
        //辅助函数, 检查传入的pObj与this的位置关系, 递归地对子树调用传入的方法method
        //pObj用来标定Obj的位置
        bool recursionForInnerGrid(gObjType *pObj, bool (Grid<gObjType>::*method)(Args...), Args... args );
        void setParent(Grid<gObjType*> p) { parentGrid = p;}
        //判断pobj指向的对象是否在方格内部
        bool Inside(const gObjType* pobj) const{
            return rect.Inside(pobj->getCrd());
        }
        inline void reInsert(gObjType* pt);


        public :
        //通过id获得对象的引用
        inline gObjType& getObj(mID objID) const;
        bool insert(gObjType* pObj);
        inline bool erase(mID objID);
        bool erase(gObjType* pObj);
        //用于更新引用关系
        /*
        frameUpdate（更新方格对引用的持有关系）的实现思路:
        需要明确: 由于位置有更新, 无论哪种Grid都要进行update
        需要frameUpdate的对象有且仅有movable对。。应当先收集这些对象的id
        应当先对分割区间细, 采用更宽泛的边界检查的Entity对象进行update
        在Entity中无需frameUpdate的可直接排除,剩下的继续留存在表中等待其他类型方格的update
        */
        //必须从细分类到粗分类调用
        //返回当前仍可能需要更新的变量
        std::unordered_set<gObjType*>& frameUpDate(std::unordered_set<gObjType*> &toUpdate);
        //检测方格是否要分裂或者重新合并,如果需要, 执行相应操作
        void checkState();
        //创建新根Grid
        //l,r,t,b分别为左右上下四个边界的坐标值
        //v是初始化时传入的对象的集合
        static Grid<gObjType>* newGrid(gMath::axisV l,gMath::axisV r,gMath::axisV t,gMath::axisV b
            , const std::vector<gObjType*>& v);
        //创建新的根Grid
        //l,r,t,b分别为左右上下四个边界的坐标值
        static Grid<gObjType>* newGrid(gMath::axisV l,gMath::axisV r,gMath::axisV t,gMath::axisV b);
        //从根创建新的Grid
        //l,r,t,b分别为左右上下四个边界的坐标值
        static Grid<gObjType>* newGrid(gMath::axisV l,gMath::axisV r,gMath::axisV t,gMath::axisV b, Grid<gObjType>* parentGrid);
};


template <typename gObjType>
class GridManager{
private:
    std::unordered_map<mID, std::unordered_map<mID,gObjType*>> referencePool;

    std::vector<Grid<gObjType>*> gridPool;
public:
    //创建GirdManager
    //rfPoolbuckets -- 初始时哈希表桶的数量
    //gridPoolInitialVolume -- 初始预估需要的Grid的数量
    GridManager(int rfPoolbuckets, int gridPoolInitialVolume): referencePool(rfPoolbuckets), gridPool(gridPoolInitialVolume,new Grid<gObjType>()){}
    GridManager(std::tuple<int,int>& initialSize):referencePool(std::get<0>(initialSize)),gridPool(std::get<1>(initialSize)){}
    //获得引用
    gObjType* get(mID poolId, mID objId) const { return referencePool.at(poolId).at(objId);}  
    //插入到引用池
    void insert(mID poolId, gObjType* pt) { referencePool[poolId][pt->id]= pt; }
    //清除code对应的引用
    void erase(mID poolId, mID objId) { referencePool[poolId].erase(objId); }
    //释放内存用
    void clear() {referencePool.clear(); gridPool.clear();}
    //请求Grid
    Grid<gObjType>* aqurieGrid();
    //销毁Grid
    void removeGrid(Grid<gObjType>* grid);
    ~GridManager(){clear();}
    std::unordered_map<mID,gObjType*>* getGridReferences(mID id){
        return referencePool.at(id);
    }
};

class ActiveRectangle : gMath::mRectangele{
public :
    static std::vector<ActiveRectangle*> rects;
    ActiveRectangle(gMath::axisV left, gMath::axisV right, gMath::axisV top, gMath::axisV bottom):mRectangele(left,right,top,bottom){
        
        rects.push_back(this);
    }
};
std::vector<ActiveRectangle*> ActiveRectangle::rects;
#endif


