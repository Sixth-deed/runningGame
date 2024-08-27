#include "maps.h"
#include "mlogger/logger.h"
int main(){
    // 启动日志系统
    Logger::getInstance().setLogFile("log/debug/debug_out.log");
    Logger::getInstance().start();
    //初始化游戏
    mGameVirtualBase* game = testmapMap::initializeGame();
    //启动webSocket服务器,端口9002
    WebSocketServer::initServer(9002);  
    game -> GameLoop();

    // 让日志线程有机会处理日志队列
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // 停止日志系统
    Logger::getInstance().stop();
    return 0;
}
