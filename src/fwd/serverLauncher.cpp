#include "fwd/serverLauncher.h"
#include "fwd/forwarder.h"
#include <thread>
void openLink(const std::string& url) {
#ifdef _WIN32
    system(("start " + url).c_str());
#elif __linux__
    system(("xdg-open " + url).c_str());
#elif __APPLE__
    system(("open " + url).c_str());
#else
    LOG_ERROR( std::string("Unsupported OS")) ;
#endif
}
std::thread launchServer()
{
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
    return std::move(vue_thread);
}

std::condition_variable WebSocketServer::connect_cv;