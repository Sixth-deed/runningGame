#ifndef GAME_MAIN
#define GAME_MAIN
#include "lib/Gmath.h"
#include "gObj.h"
#include "lib/ObjPool.h"
#include "BaseObj.h"
#include "QuadTree.h"
#include "Constraint.h"
#include "PhysicsEngine.h"
#include "GameObjects/allObjects.h"
#include "logger/logger.h"
#include "fwd/forwarder.h"
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <cerrno>
#include <thread>
#include <chrono>
#include <unordered_set>
/*
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
*/

struct gameLoopBundle
{
    int numOfActiveMoveObj;
    std::vector<gObj *> toSend;
    gameLoopBundle() : numOfActiveMoveObj(1), toSend() {}
};

/*游戏实例虚基类，定义接口用*/
class mGameVirtualBase
{
public:
    virtual void GameLoop() = 0;
    virtual ~mGameVirtualBase() = default;

#ifdef DEBUG
    virtual void printEntityGrids() = 0;
#endif
};
/*
游戏实例基类

模板参数
ManagerT - 采用的对象管理器的类型, 需要包含所有的可能用到的对象类型
GridManagerTypes - 需要套用Grid模块管理的对象的类型, 一定要包括gObj, EntityObj, ActObj
*/
template <typename ManagerT, typename... GridManagingGameObjTypes>
class mGame : public mGameVirtualBase
{
protected:
    std::unordered_map<mID, gObj *> env;

    ManagerT mainObjManager;

    PhysicsEngine *mainEngine;

    std::tuple<GridManager<GridManagingGameObjTypes>...> GridManagers;

    const std::tuple<Grid<GridManagingGameObjTypes> *...> &rootGrids;

    std::tuple<std::vector<Grid<GridManagingGameObjTypes> *> *...> activeGridsTuple;

    void deleteActiveGridsRef();
    void ActiveGridsRefernceUpdate(std::unordered_set<mID> &toUpdate, std::unordered_set<mID> &toRelease);
    template <typename gObjType, typename... Args>
    gObjType &newObj(Args... args)
    {
        gObjType &obj = gObjType::newObj(mainObjManager, args...);
        bundle.toSend.push_back(&obj);
        env.emplace(obj.getID(), static_cast<gObj *>(&obj));
        return obj;
    }
    template <typename gObjType>
    Grid<gObjType> *rootGrid()
    {
        return std::get<Grid<gObjType> *>(rootGrids);
    }
    template <typename gObjType>
    std::vector<Grid<gObjType> *> *activeGrids()
    {
        return std::get<std::vector<Grid<gObjType> *> *>(activeGridsTuple);
    }
    void updateActiveGrids();
    void initializeActiveGrids();
    mGame(std::initializer_list<std::size_t> initialSizes, std::initializer_list<std::tuple<int, int>> gridsInitialize, const gMath::mRectangle &rect, PhysicsEngine *const engine);

    template <typename cstType, typename... Args>
    cstType *newConstraint(Args... args)
    {
        return mainEngine->getCstManager()->newConstraint<cstType>(args...);
    }
    void releaseObjs(std::unordered_set<mID> &toRelease);

    void initilizeGridMangerRfr();

    gameLoopBundle bundle;

    enum GameState
    {
        RUNNING,
        PAUSED,
        TO_QUIT
    };

    GameState gameState = RUNNING;

public:
    ~mGame();
    // 初始化游戏
    static mGame &initializeGame();

    template <std::size_t... Is>
    std::tuple<GridManager<GridManagingGameObjTypes>...> createGridManagers(std::index_sequence<Is...>, const std::tuple<int, int> *initialSizes)
    {

        return std::tuple<GridManager<GridManagingGameObjTypes>...>(GridManager<GridManagingGameObjTypes>(initialSizes[Is])...);
    }

    template <std::size_t... Is>
    std::tuple<Grid<GridManagingGameObjTypes> *...> createRootGrids(std::index_sequence<Is...>, const gMath::mRectangle &rect)
    {

        return std::tuple<Grid<GridManagingGameObjTypes> *...>(Grid<GridManagingGameObjTypes>::newGrid(rect)...);
    }
    void GameLoop();
    void solveServerMessages();
#ifdef DEBUG
    void printEntityGrids();
#endif
};
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::deleteActiveGridsRef()
{
    (delete std::get<std::vector<Grid<GridManagingGameObjTypes> *> *>(activeGridsTuple), ...);
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
        std::thread deleteThread([this]()
                                 { deleteActiveGridsRef(); });
        deleteThread.detach();
        initializeActiveGrids();
    }
}
template <typename gObjType>
void helper_InsertActiveGrids(std::vector<Grid<gObjType> *> *v_pt, Grid<gObjType> *g_pt)
{
    if (ActiveRectangle::intersectsWith(g_pt->get_rect()))
    {
        if (g_pt->is_divided())
        {
            helper_InsertActiveGrids(v_pt, g_pt->LeftTop());
            helper_InsertActiveGrids(v_pt, g_pt->RightTop());
            helper_InsertActiveGrids(v_pt, g_pt->LeftBottom());
            helper_InsertActiveGrids(v_pt, g_pt->RightBottom());
        }
        else
            v_pt->emplace_back(g_pt);
    }
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::initializeActiveGrids()
{
    ((std::get<std::vector<Grid<GridManagingGameObjTypes> *> *>(activeGridsTuple) = new std::vector<Grid<GridManagingGameObjTypes> *>()), ...);
    ((helper_InsertActiveGrids(std::get<std::vector<Grid<GridManagingGameObjTypes> *> *>(activeGridsTuple), rootGrid<GridManagingGameObjTypes>())), ...);
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
mGame<ManagerT, GridManagingGameObjTypes...>::mGame(std::initializer_list<std::size_t> initialSizes, std::initializer_list<std::tuple<int, int>> gridsInitialize, const gMath::mRectangle &rect, PhysicsEngine *const engine) : mainObjManager(initialSizes),
                                                                                                                                                                                                                                GridManagers(createGridManagers(std::make_index_sequence<sizeof...(GridManagingGameObjTypes)>(), gridsInitialize.begin())),
                                                                                                                                                                                                                                rootGrids((initilizeGridMangerRfr(),
                                                                                                                                                                                                                                           createRootGrids(std::make_index_sequence<sizeof...(GridManagingGameObjTypes)>(), rect))),
                                                                                                                                                                                                                                mainEngine(engine)
{
    initializeActiveGrids();
}
template <std::size_t... Is, typename T>
void clearHelper(std::index_sequence<Is...>, T &tuple)
{
    (std::get<Is>(tuple).clear(), ...);
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
mGame<ManagerT, GridManagingGameObjTypes...>::~mGame()
{
    mainObjManager.clear();
    clearHelper(std::make_index_sequence<sizeof...(GridManagingGameObjTypes)>(), GridManagers);
    ActiveRectangle::clear();
}

template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::releaseObjs(std::unordered_set<mID> &toRelease)
{
    for (mID id : toRelease)
    {
        gObj *pt = env[id];
        switch (pt->getType())
        {
        case mtype::gInstanceTypes::gInstance:
            // 不合理的，直接使用实例类的对象
            LOG_ERROR(std::string("cannot use instance object!"));
            break;
        case mtype::gInstanceTypes::StableRectangleObj:
            // PT_CAST
            mainObjManager.release(dynamic_cast<StableRectangleObj *>(pt));
            break;
        case mtype::gInstanceTypes::LiberalRectangleObj:
            // PT_CAST
            mainObjManager.release(dynamic_cast<LiberalRectangleObj *>(pt));
            break;
        }
        env.erase(id);
    }
}

template <typename ManagerT, typename... GridManagingGameObjTypes>
inline void mGame<ManagerT, GridManagingGameObjTypes...>::initilizeGridMangerRfr()
{
    ((Grid<GridManagingGameObjTypes>::setGridManager(&std::get<GridManager<GridManagingGameObjTypes>>(GridManagers))), ...);
}

// 60是帧率
extern std::chrono::milliseconds clocksPerFrame;

// 用于辅助GameLoop从上一次GameLoop结果获取信息的结构体
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::ActiveGridsRefernceUpdate(std::unordered_set<mID> &toUpdate, std::unordered_set<mID> &toRelease)
{
    ((std::for_each(activeGrids<GridManagingGameObjTypes>()->begin(), activeGrids<GridManagingGameObjTypes>()->end(), [&](Grid<GridManagingGameObjTypes> *grid)
                    { grid->frameUpDate(toUpdate, toRelease); })),
     ...);
}
template <typename ManagerT, typename... GridManagingGameObjTypes>
void mGame<ManagerT, GridManagingGameObjTypes...>::GameLoop()
{
    using str = std::string;

#ifdef GAME_LOOP_LOG
    unsigned int frameCount = 0;
#endif
    while (true)
    {
        if (gameState == RUNNING)
        {
            std::unordered_set<mID> toUpdate(bundle.numOfActiveMoveObj * 2);
            std::unordered_set<mID> toRelease(3);
            auto start = std::chrono::steady_clock::now();

            for (Grid<ActObj> *actobjGrid_p : *(activeGrids<ActObj>()))
            {
                actobjGrid_p->forEachInGrid([&toUpdate](ActObj *actObj_p)
                                            {
                 actObj_p->act();
                 if (actObj_p ->isMovable() && !actObj_p->isSleep()){
                    toUpdate.insert(actObj_p->getID());
                 } });
            }
            updateActiveGrids();
            std::unordered_set<mID> toupdateReference(toUpdate.begin(), toUpdate.end());
            ActiveGridsRefernceUpdate(toupdateReference, toRelease);
            releaseObjs(toRelease);

#ifdef DEBUG
            //   LOG_DEBUG(str("in GameLoop EntityGrids: \n\t"));
            //       printEntityGrids();
#endif
            // 约束修正
            for (Grid<EntityObj> *ettobjGrid_p : *(activeGrids<EntityObj>()))
            {
                ettobjGrid_p->forEachInGrid([&toUpdate, ettobjGrid_p, this](EntityObj *ettObj_p)
                                            {
        auto& mainEngine = this->mainEngine;
            if (ettObj_p->isMovable()){
                if (toUpdate.count(ettObj_p->getID())){
                    ettobjGrid_p -> forEachInGrid([ettObj_p , mainEngine](EntityObj* other){
                            mainEngine->getCstManager()->checkAndUpdate(ettObj_p, other);
                            
                    });
                }
            } });
            }
            mainEngine->getCstManager()->updateAll();
            mainEngine->getCstManager()->solveAll();

            solveServerMessages();

#ifdef GAME_LOOP_LOG
#ifdef DEBUG
            if ((frameCount++) % 30 == 0)
            {
                LOG_DEBUG("FrameCount: " + std::to_string(frameCount) + "\n");
                for (auto &pair : env)
                {
                    LOG_DEBUG("A game object:" + pair.second->log());
                }
            }
#else
            if ((frameCount++) % 60 == 0)
            {
                LOG_INFO("FrameCount:" + std::to_string(frameCount) + " !\n");
                for (gObj *gObj_p : bundle.toSend)
                {
                    LOG_INFO("A game object:" + gObj_p->log());
                }
            }
#endif
#endif

            for (gObj *gobj : bundle.toSend)
            {
                WebSocketServer::send_obj(gobj);
            }
            bundle.toSend.clear();

            auto end = std::chrono::steady_clock::now();
            auto elasped = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            if (elasped < clocksPerFrame)
                std::this_thread::sleep_for(clocksPerFrame - elasped);
        }
        else if (gameState == PAUSED)
        {
            solveServerMessages();
        }
        else if (gameState == TO_QUIT)
        {
            break;
        }
    }
}

template <typename ManagerT, typename... GridManagingGameObjTypes>
inline void mGame<ManagerT, GridManagingGameObjTypes...>::solveServerMessages()
{
    std::queue<unsigned int> &msgQueue = WebSocketServer::get_mes_queue();
    while (!msgQueue.empty())
    {
        unsigned int command = msgQueue.front();
        switch (command)
        {
        case 0:
            gameState = GameState::PAUSED;
            break;
        case 1:
            gameState = GameState::RUNNING;
            break;
        case 2:
            gameState = GameState::TO_QUIT;
            break;
        default:
            break;
        }
        msgQueue.pop();
    }
}

template <typename ManagerT, typename... GridManagingGameObjTypes>
inline void mGame<ManagerT, GridManagingGameObjTypes...>::printEntityGrids()
{
    for (Grid<EntityObj> *ettobjGrid_p : *(activeGrids<EntityObj>()))
    {
        LOG_DEBUG("EntityGrid: \n\t" + ettobjGrid_p->log() + "\n");
        LOG_DEBUG(std::string("Objs in grid: \n\t"));
        ettobjGrid_p->forEachInGrid([](EntityObj *ettObj_p)
                                    { LOG_DEBUG("EntityObj: \n\t" + ettObj_p->log() + "\n"); });
    }
}
#endif