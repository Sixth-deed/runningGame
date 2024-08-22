#include "QuadTree.h"

    template <typename gObjType>
void Grid<gObjType>::divideSelf()
{
    gMath::axisV avLR = (rect.l + rect.r) / 2 , avTB=(rect.t+rect.b)/2;
    lt = newGrid(rect.l ,avLR , rect.t, avTB ,this);
    rt = newGrid(avLR, rect.r, rect.t, avTB, this);
    lb = newGrid(rect.l, avLR, avTB, rect.b, this);
    rb = newGrid(avLR, rect.r, avTB, rect.b, this);
    for (auto pObj : this->mManager.getGridReferences(this->gridID)){
        recursionForInnerGrid(pObj, insert, pObj);
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
void Grid<gObjType>::collectFrom(const Grid<gObjType>* pGrid)
{
    for (auto pObj : this->mManager->getGridReferences(pGrid)){
        insert(pGrid);
    }
}

    template <typename gObjType>
std::unordered_set<gObjType*>& Grid<gObjType>::frameUpDate(std::unordered_set<gObjType*> &toUpdate, std::unordered_set<gObjType*>& toRelease)
{

    for (auto pt : toUpdate){
        if (count(pt->id)){
            if (Inside(pt))
                toUpdate.erase(pt);
            else{
                bool insideFlag=false;
                for (auto prt : ActiveRectangle::rects){
                    if (prt->Inside(pt->getCrd())){
                        insideFlag = true;
                        break;
                    }
                }
                if (insideFlag)
                    reInsert(pt);
                else 
                    //MoveObj的方法
                    if (!pt->onOffTackler()){
                        erase(pt);
                        toUpdate.erase(pt);
                        toRelease.insert(pt);                        
                    }
            }
        }

    }
    return toUpdate;
}

    template <typename gObjType>
void Grid<gObjType>::checkState()
{
    if (divided){
        if (this->mManager.getGridReferences(this->gridID).size() <= this->threshold){
            collectSelf();
        }
    }
    else{
        if (this->mManager.getGridReferences(this->gridID).size() > this->threshold){
            divideSelf();
        }
    }
}




    template <typename T>
Grid<T>::Grid(gMath::axisV l_, gMath::axisV r_, gMath::axisV t_, gMath::axisV b_, const std::vector<T *> &v) : rect(l_,r_,t_,b_)
{
    //将vector中的对象向引用池中绑定
    for (T* pt : v){
        insert(pt);
    }
}
template <typename T>
idHandler Grid<T>::gIDhdr;




    template <typename gObjType>
bool Grid<gObjType>::insert(gObjType *pObj)
{

    if (divided){
        return recursionForInnerGrid( pObj , &insert , pObj);
    }
    else{
        mManager->insert(gridID , pObj);
        return true;
    }
}



    template <typename gObjType>
bool Grid<gObjType>::erase(gObjType *pObj)
{

    if (divided){
        return recursionForInnerGrid( pObj , &insert , pObj);
    }
    else{
        mManager->erase(gridID,pObj->id);
        return true;
    }
}

    template <typename gObjType>
Grid<gObjType> *Grid<gObjType>::newGrid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b, const std::vector<gObjType *> &v)
{
    auto pObj = mManager->aquireGrid();
    pObj->rect.l=l,pObj->rect.r=r,pObj->rect.t=t,pObj->rect.b=b;
    for (auto pt : v){
        mManager->insert(pObj->gridID,pt->id);
    }
    return pObj;
}

    template <typename gObjType>
Grid<gObjType> *Grid<gObjType>::newGrid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b)
{
    auto pObj = mManager->aquireGrid();
    pObj->rect.l=l,pObj->rect.r=r,pObj->rect.t=t,pObj->rect.b=b;
    pObj->parentGrid = nullptr , pObj->root = pObj;
    return pObj;
}

    template <typename gObjType>
Grid<gObjType> *Grid<gObjType>::newGrid(gMath::axisV l, gMath::axisV r, gMath::axisV t, gMath::axisV b, Grid<gObjType> *parentGrid)
{
    auto pObj = mManager->aquireGrid();
    pObj->rect.l=l,pObj->rect.r=r,pObj->rect.t=t,pObj->rect.b=b;
    pObj->parentGrid = parentGrid, pObj -> root = parentGrid->root;
    return pObj;
}

    template <typename T>
Grid<T> *GridManager<T>::aqurieGrid()
{
    if (gridPool.empty()){
        return new Grid<T>();
    }    
    else{
        auto grid=gridPool.back();
        gridPool.pop_back();
        return grid;
    }
    return nullptr;
}



template <typename gObjType>
    template <typename... Args>
bool Grid<gObjType>::recursionForInnerGrid(gObjType *pObj, bool (Grid<gObjType>::*method)(Args...), Args... args)
{
    //对象在坐标轴上将归为右边的, 上方的方块中
    //编码:
    //00: rt 右上
    //01: lt 左上
    //10: rb 右下
    //11: lb 左下
    short state = (pObj.get_x() * 2 - rect.l - rect.r < 0) + (pObj.get_y()*2 - rect.t - rect.b < 0) *2;

    switch(state){
        case 0:
            return (rt->*method)(args...);
        case 1:
            return (lt->*method)(args...);
        case 2:
            return (rb->*method)(args...);
        case 3:
            return (lb->*method)(args...);
    }
}

