#include "GameMain.h"
#include "Gmath.h"
#include <thread>
#include <ctime>
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::deleteActiveGridsRef()
{
    (delete std::get<GridManagingGameObjTypes>(activeGridsTuple), ...);
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::upDateActiveGrids()
{
    bool re = false;
    for (auto activeRect : ActiveRectangle::rects)
    {
        re || = activeRect->is_moved();
    }
    if (re)
    {
        std::thread deleteThread([]()
                                 { deleteActiveGridsRef(std::make_index_sequence<sizeof...(GridManagingGameObjTypes)>()); });
        deleteThread.detach();
        initializeActiveGrids();
    }
}
template <typename gObjType>
void helper_InsertActiveGrids(vector<Grid<gObjType>*>* v_pt, Grid<gObjType>* g_pt){
    if (ActiveRectangle::intersectsWith(g_pt->rect)){
        if (g_pt->is_divided()){
            helper_InsertActiveGrids(v_pt ,g_pt->LeftTop());
            helper_InsertActiveGrids(v_pt ,g_pt->RightTop());
            helper_InsertActiveGrids(v_pt ,g_pt->LeftBottom());
            helper_InsertActiveGrids(v_pt ,g_pt->RightBottom());
        }
        else
            v_pt->emplace(g_pt);
    }
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::initializeActiveGrids()
{
    (std::get<GridManagingGameObjTypes>(activeGridsTuple) = new vector<Grid<GridManagingGameObjTypes>*>(),...)
    (helper_InsertActiveGrids(std::get<GridManagingGameObjTypes>(activeGridsTuple),rootGrid<GridManagingGameObjTypes>),...)
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
mGame<ManagerT, GridManagingGameObjTypes...>::mGame(std::initializer_list<std::size_t> initialSizes, std::initializer_list<std::tuple<int, int>> gridsInitialize, const gMath::mRectangele &rect) : mainObjManager(initialSizes),
                                                                                                                                                                                                    GridManagers(createGirdManagers(std::make_index_sequence<sizeof...(GridManagingGameObjTypes)>(), gridsInitialize.begin())),
                                                                                                                                                                                                    rootGrids(createRootGrids(std::make_index_sequence<sizeof...(GridManagingGameObjTypes)>(), rect))
{
}
template <std::size_t... Is, typename T>
void clearHelper(std::index_sequence<Is...>, T &tuple)
{
    (std::get<Is>(tuple)->clear(), ...);
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
mGame<ManagerT, GridManagingGameObjTypes...>::~mGame()
{
    mainObjManager.clear();
    clearHelper(std::make_index_sequence<sizeof...(GridManagingGameObjTypes)>(), rootGrids);
    ActiveRectangle::clear();
}

//60是帧率
clock_t clocksPerFrame = ceil(CLOCKS_PER_SEC/ 60);

template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::GameLoop()
{
    clock_t start = clock();
    for (Grid<MoveObj> *moveObjGrid_p : activeGrids<MoveObj>())
    {
        moveObjGrid_p->forEachInGrid([](MoveObj *moveObj_p)
            {
            //需实现
            moveObj_p->updatePosition(); 
            });
    }
    updateActiveGrids();
    //约束修正
    //...

    //遍历碰撞Grid

    //处理碰撞

    //处理接收到的操作
    auto commandQueue = aquireCommandBuffer();
    //更新加速度与速度


    clock_t end = clock();
    if (end - start < clocksPerFrame)
        std::this_thread::sleep_for(clocksPerFrame- end + start) 
    
}
