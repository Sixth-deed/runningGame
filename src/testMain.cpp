#include "fwd/serverLauncher.h"
#include "maps.h"
#include "mlogger/logger.h"
#include <cstdlib>
#include <thread>

int main(){

   // 启动日志系统
    mGameLogger::getInstance().setLogFile("");
    mGameLogger::getInstance().start();
    //初始化游戏
    mGameVirtualBase* game = testmapMap::initializeGame();
    //启动webSocket服务器,端口9002
    std::thread server_thread([](){WebSocketServer::initServer(9002);}); 

    // 启动前端
    std::thread vue_thread = launchServer();


    std::unique_lock<std::mutex> lk(WebSocketServer::get_server()->connectMutex);
    WebSocketServer::connect_cv.wait(lk, []{ return WebSocketServer::get_server()->connectionStarted; }); // 阻塞直到连接成功建立为true

    game -> GameLoop();

    // 让日志线程有机会处理日志队列
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // 停止日志系统
    mGameLogger::getInstance().stop();

    if (vue_thread.joinable()){
        vue_thread.join();
    }
    if (server_thread.joinable()){
        server_thread.join();
    }
    return 0;
}
