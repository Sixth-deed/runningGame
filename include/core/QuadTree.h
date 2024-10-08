#ifndef QUADTREE_GRID_AND_REFERENCE_MANAGER
#define QUADTREE_GRID_AND_REFERENCE_MANAGER

// 采用四叉树管理整个地图的对象引用分配
#include "lib/Gmath.h"
#include "lib/idHandler.h"
#include "BaseObj.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
// 方格类，用于四叉树优化
template <typename gObjType>
class Grid;
// 管理方格的辅助类
template <typename gObjType>
class GridManager;

// 用于区分活跃部分和不活跃部分
class ActiveRectangle;
class gObj;

template <typename gObjType>
class Grid
{
    static_assert(std::is_base_of<gObj, gObjType>::value, "the template type parameter must be a subclass of gObj!");

private:
    // left, right, top, bottom
    gMath::mRectangle rect;
    // left top, right top, left top, right top
    Grid<gObjType> *lt, *rt, *lb, *rb;
    // 如果分割自某个方格持有其指针
    Grid<gObjType> *parentGrid;
    // 持有根指针
    Grid<gObjType> *root;

    // 是否已经被分割
    bool divided = false;
    // 当前方格的id
    mID gridID;
    // id管理器
    static idHandler gIDhdr;

    // 指向manager的指针
    static GridManager<gObjType> *mManager;

    // 持有的对象数字达到这个数就分割
    static const unsigned int threshold;
    // 分割用的函数
    void divideSelf();
    // 合并用函数
    void collectSelf();
    // 辅助函数, 把inputGrid的引用全部添加到this的引用池中
    void collectFrom(const Grid<gObjType> *);
    // 默认构造函数, GridManager 构造新对象时调用
    Grid() : rect(0, 0, 0, 0), gridID(gIDhdr.getNewID()) {}
    Grid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b, const std::vector<gObjType *> &v);
    template <typename... Args>
    // 辅助函数, 检查传入的pObj与this的位置关系, 递归地对子树调用传入的方法method
    // pObj用来标定Obj的位置
    bool recursionForInnerGrid(gObjType *pObj, bool (Grid<gObjType>::*method)(Args...), Args... args);
    void setParent(Grid<gObjType> *p) { parentGrid = p; }

    // 判断pobj指向的对象是否在方格内部
    bool Inside(const gObj *pobj) const
    {
        return rect.Inside(pobj->getCrd());
    }
    bool Inside(const EntityObj *pobj) const
    {
        return rect.Inside(pobj->getCrd());
    }
    inline void reInsert(gObjType *pt);

public:
    template <typename T>
    friend class GridManager;

    // 通过id获得对象的引用
    inline gObjType &getObj(mID objID) const;
    bool insert(gObjType *pObj);
    inline bool erase(mID objID);
    bool erase(gObjType *pObj);
    // 用于更新引用关系
    /*
    frameUpdate（更新方格对引用的持有关系）的实现思路:
    需要明确: 由于位置有更新, 无论哪种Grid都要进行update
    需要frameUpdate的对象有且仅有movable对。。应当先收集这些对象的id
    应当先对分割区间细, 采用更宽泛的边界检查的Entity对象进行update
    在Entity中无需frameUpdate的可直接排除,剩下的继续留存在表中等待其他类型方格的update
    */
    // 必须从细分类到粗分类调用
    // 返回当前仍可能需要更新的变量
    std::unordered_set<mID> &frameUpDate(std::unordered_set<mID> &toUpdate, std::unordered_set<mID> &toRelease);
    // 检测方格是否要分裂或者重新合并,如果需要, 执行相应操作
    void checkState();
    // 创建新根Grid
    // l,r,t,b分别为左右上下四个边界的坐标值
    // v是初始化时传入的对象的集合
    static Grid<gObjType> *newGrid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b, const std::vector<gObjType *> &v);
    // 创建新的根Grid
    // l,r,t,b分别为左右上下四个边界的坐标值
    static Grid<gObjType> *newGrid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b);
    static Grid<gObjType> *newGrid(const gMath::mRectangle &rect);
    // 从根创建新的Grid
    // l,r,t,b分别为左右上下四个边界的坐标值
    static Grid<gObjType> *newGrid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b, Grid<gObjType> *parentGrid);
    template <typename FuncType>
    void forEachInGrid(FuncType f)
    {
        std::for_each(mManager->getGridReferences(gridID).begin(), mManager->getGridReferences(gridID).end(), [f](const std::pair<mID, gObjType *> &pr)
                      { f(pr.second); });
    }
    bool is_divided()
    {
        return divided;
    }
    Grid<gObjType> *LeftTop()
    {
        return lt;
    }
    Grid<gObjType> *RightTop()
    {
        return rt;
    }
    Grid<gObjType> *LeftBottom()
    {
        return lb;
    }
    Grid<gObjType> *RightBottom()
    {
        return rb;
    }
    int count(mID objId)
    {
        return mManager->count(gridID, objId);
    }
    const gMath::mRectangle &get_rect() const
    {
        return rect;
    }
    static void setGridManager(GridManager<gObjType> *m) { mManager = m; }

    std::string log() const
    {   
        return std::string("{\n") +  
        "   gridID: " + std::to_string(gridID) +",\n "
        #ifdef DEBUG 
        "   rect: " + rect.log() + ",\n " +
        #endif 
        "   divided: " + std::to_string(divided) +  "\n" + 
        "   objectCnt = " + std::to_string(mManager->getGridReferences(gridID).size()) + "\n}";
    }
};

template <>
bool Grid<EntityObj>::Inside(const gObj *pt) const;

template <typename gObjType>
class GridManager
{
private:
    std::unordered_map<mID, std::unordered_map<mID, gObjType *>> referencePool;

    std::vector<Grid<gObjType> *> gridPool;

public:
    // 创建GirdManager
    // rfPoolbuckets -- 初始时哈希表桶的数量
    // gridPoolInitialVolume -- 初始预估需要的Grid的数量

    /**
     * Constructs a GridManager instance with the specified initial size.
     *
     * @param initialSize A tuple containing the initial size of the reference pool and the grid pool.
     *
     * @note The reference pool size is doubled to accommodate for potential growth.
     */
    GridManager(const std::tuple<int, int> &initialSize) : referencePool(std::get<0>(initialSize) * 2), gridPool(std::get<1>(initialSize))
    {
        const int gpSize = std::get<1>(initialSize);
        for (int i = 0 ; i < gpSize; i++){
            gridPool[i] = new Grid<gObjType>();
        }
    }
    // 获得引用
    gObjType *get(mID poolId, mID objId) const { return referencePool.at(poolId).at(objId); }
    // 插入到引用池
    void insert(mID poolId, gObjType *pt) { referencePool[poolId][pt->getID()] = pt; }
    // 清除code对应的引用
    void erase(mID poolId, mID objId) { referencePool[poolId].erase(objId); }
    // 释放内存用
    void clear()
    {
        referencePool.clear();
        gridPool.clear();
    }
    int count(mID poolid, mID objId)
    {
        return referencePool[poolid].count(objId);
    }
    // 请求Grid
    Grid<gObjType> *acquireGrid();
    // 销毁Grid
    void removeGrid(Grid<gObjType> *grid);
    ~GridManager() { clear(); }
    std::unordered_map<mID, gObjType *> &getGridReferences(mID id)
    {
        return referencePool[id];
    }
};

template <typename T>
GridManager<T> *Grid<T>::mManager = nullptr;

template <typename T>
const unsigned int Grid<T>::threshold = 50;

template <>
const unsigned int Grid<gObj>::threshold;

template <typename T>
inline void GridManager<T>::removeGrid(Grid<T> *grid)
{
    gridPool.push_back(grid);
    referencePool[grid->gridID].clear();
}

template <typename gObjType>
inline bool Grid<gObjType>::erase(mID objID)
{
    return erase(&getObj(objID));
}

template <typename T>
inline T &Grid<T>::getObj(mID objID) const
{
    return *((T *)((*mManager).get(gridID, objID)));
}

template <typename gObjType>
inline void Grid<gObjType>::reInsert(gObjType *pt)
{
    root->insert(pt);
}

template <typename gObjType>
void Grid<gObjType>::divideSelf()
{
    gMath::axisV avLR = (rect.l + rect.r) / 2, avTB = (rect.t + rect.b) / 2;
    lt = newGrid(rect.l, avLR, rect.t, avTB, this);
    rt = newGrid(avLR, rect.r, rect.t, avTB, this);
    lb = newGrid(rect.l, avLR, avTB, rect.b, this);
    rb = newGrid(avLR, rect.r, avTB, rect.b, this);
    for (auto pObj : this->mManager->getGridReferences(this->gridID))
    {
        recursionForInnerGrid(pObj.second, &Grid<gObjType>::insert, pObj.second);
    }
    {
        lt->checkState();
        rt->checkState();
        lb->checkState();
        rb->checkState();
    }
}

template <typename gObjType>
void Grid<gObjType>::collectSelf()
{
    divided = false;
    collectFrom(lt);
    collectFrom(rt);
    collectFrom(lb);
    collectFrom(rb);
    mManager->removeGrid(lt);
    mManager->removeGrid(rt);
    mManager->removeGrid(lb);
    mManager->removeGrid(rb);
}

template <typename gObjType>
void Grid<gObjType>::collectFrom(const Grid<gObjType> *pGrid)
{
    for (auto pObj : this->mManager->getGridReferences(pGrid->gridID))
    {
        insert(pObj.second);
    }
}
// 使用inline才能编译通过
template <typename gObjType>
inline std::unordered_set<mID> &Grid<gObjType>::frameUpDate(std::unordered_set<mID> &toUpdate, std::unordered_set<mID> &toRelease)
{
    std::unordered_set<mID> toErase;
    for (mID id : toUpdate)
    {
        if (count(id))
        {
            gObjType& obj = getObj(id);
            if (Inside(&obj))
                toErase.insert(id);
            else
            {
                bool insideFlag = false;
                for (auto &prt : ActiveRectangle::rects)
                {
                    if (prt->Inside(obj.getCrd()))
                    {
                        insideFlag = true;
                        break;
                    }
                }
                if (insideFlag)
                    reInsert(&obj);
            }
        }
    }
    for (mID id : toErase){
        toUpdate.erase(id);
    }
    return toUpdate;
}
template <>
inline std::unordered_set<mID> &Grid<ActObj>::frameUpDate(std::unordered_set<mID> &toUpdate, std::unordered_set<mID> &toRelease)
{
    std::unordered_set<mID> toErase;
    for (mID id : toUpdate)
    {
        if (count(id))
        {
            ActObj& obj = getObj(id);
            if (Inside(&obj))
                toErase.insert(id);
            else
            {
                bool insideFlag = false;
                for (auto &prt : ActiveRectangle::rects)
                {
                    if (prt->Inside(obj.getCrd()))
                    {
                        insideFlag = true;
                        break;
                    }
                }
                if (insideFlag)
                    reInsert(&obj);
                else
                    // ActObj的虚方法，MoveObj具体实现，只有MoveObj会进入到这一步
                    if (obj.onOffTackler())
                    {
                        erase(id);
                        toErase.insert(id);
                        toRelease.insert(id);
                    }
            }
        }
    }
    for (mID id : toErase){
        toUpdate.erase(id);
    }
    return toUpdate;
}
template <typename gObjType>
void Grid<gObjType>::checkState()
{
    if (divided)
    {
        if (this->mManager->getGridReferences(this->gridID).size() <= this->threshold)
        {
            collectSelf();
        }
    }
    else
    {
        if (this->mManager->getGridReferences(this->gridID).size() > this->threshold)
        {
            divideSelf();
        }
    }
}

template <typename T>
Grid<T>::Grid(gMath::axisV l_, gMath::axisV r_, gMath::axisV t_, gMath::axisV b_, const std::vector<T *> &v) : rect(l_, r_, t_, b_)
{
    // 将vector中的对象向引用池中绑定
    for (T *pt : v)
    {
        insert(pt);
    }
}
template <typename T>
idHandler Grid<T>::gIDhdr;

template <typename gObjType>
bool Grid<gObjType>::insert(gObjType *pObj)
{

    if (divided)
    {
        return recursionForInnerGrid(pObj, &Grid<gObjType>::insert, pObj);
    }
    else
    {
        mManager->insert(gridID, pObj);
        return true;
    }
}

template <typename gObjType>
bool Grid<gObjType>::erase(gObjType *pObj)
{

    if (divided)
    {
        return recursionForInnerGrid(pObj, &Grid<gObjType>::insert, pObj);
    }
    else
    {
        mManager->erase(gridID, pObj->getID());
        return true;
    }
}

template <typename gObjType>
Grid<gObjType> *Grid<gObjType>::newGrid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b, const std::vector<gObjType *> &v)
{
    auto pObj = mManager->acquireGrid();
    pObj->rect.l = l, pObj->rect.r = r, pObj->rect.t = t, pObj->rect.b = b;
    for (auto pt : v)
    {
        mManager->insert(pObj->gridID, pt);
    }
    return pObj;
}

template <typename gObjType>
Grid<gObjType> *Grid<gObjType>::newGrid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b)
{
    auto pObj = mManager->acquireGrid();
    pObj->rect.l = l, pObj->rect.r = r, pObj->rect.t = t, pObj->rect.b = b;
    pObj->parentGrid = nullptr, pObj->root = pObj;
    return pObj;
}
template <typename gObjType>
Grid<gObjType> *Grid<gObjType>::newGrid(const gMath::mRectangle &rect)
{
    auto pObj = mManager->acquireGrid();
    pObj->rect = rect;
    pObj->parentGrid = nullptr, pObj->root = pObj;
    return pObj;
}

template <typename gObjType>
Grid<gObjType> *Grid<gObjType>::newGrid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b, Grid<gObjType> *parentGrid)
{
    auto pObj = mManager->acquireGrid();
    pObj->rect.l = l, pObj->rect.r = r, pObj->rect.t = t, pObj->rect.b = b;
    pObj->parentGrid = parentGrid, pObj->root = parentGrid->root;
    return pObj;
}

template <typename T>
Grid<T> *GridManager<T>::acquireGrid()
{
    Grid<T> *pt = nullptr;
    if (gridPool.empty())
    {
        pt = new Grid<T>();
    }
    else
    {
        pt = gridPool.back();
        gridPool.pop_back();
    }
    referencePool[pt->gridID] = std::unordered_map<mID, T *>();
    return pt;
}

template <typename gObjType>
template <typename... Args>
bool Grid<gObjType>::recursionForInnerGrid(gObjType *pObj, bool (Grid<gObjType>::*method)(Args...), Args... args)
{
    // 对象在坐标轴上将归为右边的, 上方的方块中
    // 编码:
    // 00: rt 右上
    // 01: lt 左上
    // 10: rb 右下
    // 11: lb 左下
    short state = (pObj->get_x() * 2 - rect.l - rect.r < 0) + (pObj->get_y() * 2 - rect.t - rect.b < 0) * 2;

    switch (state)
    {
    case 0:
        return (rt->*method)(args...);
    case 1:
        return (lt->*method)(args...);
    case 2:
        return (rb->*method)(args...);
    case 3:
        return (lb->*method)(args...);
    }
    return false;
}
template<>
inline bool Grid<EntityObj>::Inside(const EntityObj *pt) const{
    return rect.Inside( pt->getCrd() ,pt->get_c_CollisionBox().getShape_c());
}
#endif
