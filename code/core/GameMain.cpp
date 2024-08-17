#include "GameMain.h"
#include "BaseObj.h"
#include "lib/Gmath.h"
#include "../fwd/forwarder.h"
#include <algorithm>
#include <cerrno>
#include <thread>
#include <chrono>
#include <unordered_set>
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::deleteActiveGridsRef()
{
    (delete std::get<GridManagingGameObjTypes>(activeGridsTuple), ...);
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::updateActiveGrids()
{
    bool re = false;
    for (auto activeRect : ActiveRectangle::rects)
    {
        re |= activeRect->is_moved();
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
void helper_InsertActiveGrids(std::vector<Grid<gObjType> *> *v_pt, Grid<gObjType> *g_pt)
{
    if (ActiveRectangle::intersectsWith(g_pt->rect))
    {
        if (g_pt->is_divided())
        {
            helper_InsertActiveGrids(v_pt, g_pt->LeftTop());
            helper_InsertActiveGrids(v_pt, g_pt->RightTop());
            helper_InsertActiveGrids(v_pt, g_pt->LeftBottom());
            helper_InsertActiveGrids(v_pt, g_pt->RightBottom());
        }
        else
            v_pt->emplace(g_pt);
    }
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::initializeActiveGrids()
{
    ((std::get<GridManagingGameObjTypes>(activeGridsTuple) = new std::vector<Grid<GridManagingGameObjTypes> *>()), ...);
    ((helper_InsertActiveGrids(std::get<GridManagingGameObjTypes>(activeGridsTuple), rootGrid<GridManagingGameObjTypes>)), ...);
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
mGame<ManagerT, GridManagingGameObjTypes...>::mGame(std::initializer_list<std::size_t> initialSizes, std::initializer_list<std::tuple<int, int>> gridsInitialize, const gMath::mRectangle &rect, PhysicsEngine* const engine) : 
    mainObjManager(initialSizes),
    GridManagers(createGirdManagers(std::make_index_sequence<sizeof...(GridManagingGameObjTypes)>(), gridsInitialize.begin())),
    rootGrids(createRootGrids(std::make_index_sequence<sizeof...(GridManagingGameObjTypes)>(), rect)),
    mainEngine(engine)
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

// 60是帧率
std::chrono::milliseconds clocksPerFrame(1000 / 60);

// 用于辅助GameLoop从上一次GameLoop结果获取信息的结构体
struct gameLoopParam
{
    int numOfActiveMoveObj;
};

template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::ActiveGridsRefernceUpdate(std::unordered_set<MoveObj *> &toUpdate, std::unordered_set<MoveObj *> &toRelease)
{
    ((std::for_each(activeGrids<GridManagingGameObjTypes>()->begin(), activeGrids<GridManagingGameObjTypes>->end(), [&](Grid<GridManagingGameObjTypes> *grid)
                    { grid->frameUpDate(toUpdate, toRelease); })),
     ...);
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::GameLoop(gameLoopParam &param)
{
    std::unordered_set<MoveObj *> toUpdate(param.numOfActiveMoveObj * 2);
    std::unordered_set<MoveObj *> toRelease(3);
    auto start = std::chrono::steady_clock::now();
    for (Grid<ActObj> *actobjGrid_p : activeGrids<ActObj>())
    {
        actobjGrid_p->forEachInGrid([&toUpdate](ActObj *actObj_p)
                                    {
                 actObj_p->act();
                 if (actObj_p ->movable){
                    toUpdate.insert(reinterpret_cast<MoveObj*>(actObj_p));
                 } });
    }
    updateActiveGrids();
    std::unordered_set<MoveObj *> toupdateReference(toUpdate.begin(), toUpdate.end());
    ActiveGridsRefernceUpdate(toupdateReference, toRelease);
    releaseObjs(toRelease);

    // 约束修正
    for (Grid<EntityObj> *ettobjGrid_p : activeGrids<EntityObj>())
    {
        ettobjGrid_p->forEachInGrid([const &toUpdate, ettobjGrid_p, mainEngine](EntityObj *ettObj_p)
        {
            if (ettObj_p->movable){
                if (toUpdate.count(reinterpret_cast<MoveObj*>(ettObj_p))){
                    ettobjGrid_p -> forEachInGrid([ettObj_p , mainEngine](EntityObj* other){
                            mainEngine->getCstManager()->checkAndUpdate(ettObj_p, other);
                            
                    });
                }
            } });
    }
    mainEngine->getCstManager()->updateAll();
    mainEngine->getCstManager()->solveAll();

    // 处理接收到的操作
    auto commandQueue = aquireCommandBuffer();
    while (!commandQueue.empty())
    {
        auto &command = commandQueue.top();
        commandQueue.pop();
        command.solve();
    }

    // 向前端发信
    
    //toSend会有的，不用考虑实现
    //只发送改变了位置或状态(flags)的对象
    gObjContainer toSend;  
    SendPackContainer mesContainer

    for (auto gobj : toSend){
        gobj.send(mesContainer);
    }
    // 交给webSocket模块处理容器
    // 在code/fwd/ 目录下实现收发细节
    webSend(mesContainer);

    //这只是一个示例，可以通过别的方法实现，比如在obj的send函数里就直接调用websocket的收发



    auto end = std::chrono::steady_clock::now();
    auto elasped = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if (elasped < clocksPerFrame)
        std::this_thread::sleep_for(clocksPerFrame - elasped);
    GameLoop(param);
}
