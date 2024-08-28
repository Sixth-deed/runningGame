#include "httplib.h"
#include "maps.h"
#include "mlogger/logger.h"
#include <cstdlib>
#include <thread>
void openLink(const std::string& url) {
#ifdef _WIN32
    system(("start " + url).c_str());
#elif __linux__
    system(("xdg-open " + url).c_str());
#elif __APPLE__
    system(("open " + url).c_str());
#else
    std::cerr << "Unsupported OS" << std::endl;
#endif
}
int main(){

   // 启动日志系统
    mGameLogger::getInstance().setLogFile("log/debug/debug_out.log");
    mGameLogger::getInstance().start();
    //初始化游戏
    mGameVirtualBase* game = testmapMap::initializeGame();
    //启动webSocket服务器,端口9002
    std::thread server_thread([](){WebSocketServer::initServer(9002);}); 

#ifdef FRONTEND_DEBUG 
    //前端调试启动
 
    std::thread vue_thread([](){system("cd vue-test/ && npm run dev");});
    openLink("http://localhost:5173/"); 
#else

    //启动发布版vue体系
    std::thread vue_thread([](){
        httplib::Server svr;
        svr.set_mount_point("/", "./vue-test/dist");
        svr.listen("0.0.0.0", 8080);

     });
    
    openLink("http://localhost:8080/"); 
#endif
 
    game -> GameLoop();

    // 让日志线程有机会处理日志队列
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // 停止日志系统
    mGameLogger::getInstance().stop();
    sleep(100);
    if (vue_thread.joinable()){
        vue_thread.join();
    }
    if (server_thread.joinable()){
        server_thread.join();
    }
    return 0;
}
