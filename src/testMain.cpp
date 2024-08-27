#include "maps.h"
#include "mlogger/logger.h"
#include <cstdlib>
#include <thread>
int main(){
    std::thread vue_thread([](){system("cd vue-test/ && npm run dev");});
    system("xdg-open http://localhost:5173/"); 
    system("pwd");
    system("npm run dev");
    // 启动日志系统
    Logger::getInstance().setLogFile("log/debug/debug_out.log");
    Logger::getInstance().start();
    //初始化游戏
    mGameVirtualBase* game = testmapMap::initializeGame();
    //启动webSocket服务器,端口9002
    std::thread server_thread([](){WebSocketServer::initServer(9002);}); 
    game -> GameLoop();

    // 让日志线程有机会处理日志队列
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // 停止日志系统
    Logger::getInstance().stop();
    sleep(100);
    if (vue_thread.joinable()){
        vue_thread.join();
    }
    if (server_thread.joinable()){
        server_thread.join();
    }
    return 0;
}
