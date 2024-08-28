#include "core/gObj.h"
#include <cstdint>
#include <mutex>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <set>
#include <queue>
#include <string>
#include <functional>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using json = nlohmann::json;

json to_json(const gObj& obj);

class WebSocketServer {
public:
    WebSocketServer() {
        // 初始化 WebSocket 服务器
        m_server.init_asio();

        // 设置打开连接的回调函数
        m_server.set_open_handler(bind(&WebSocketServer::on_open, this, std::placeholders::_1));

        // 设置关闭连接的回调函数
        m_server.set_close_handler(bind(&WebSocketServer::on_close, this, std::placeholders::_1));

        // 设置消息处理的回调函数
        m_server.set_message_handler(bind(&WebSocketServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
    }

    void run(uint16_t port) {
        // 监听指定端口
        m_server.listen(port);

        // 启动 WebSocket 服务器
        m_server.start_accept();

        // 运行服务器
        m_server.run();
    }

    void on_open(connection_hdl hdl) {
        // 连接打开时的处理
        LOG_DEBUG (std::string("Connection opened") );
        m_connections.insert(hdl);
        std::lock_guard<std::mutex> lk(connectMutex);
        connectionStarted = true;
        connect_cv.notify_one();
    }

    void on_close(connection_hdl hdl) {
        // 连接关闭时的处理
        LOG_DEBUG (std::string("Connection closed") );
        m_connections.erase(hdl);
    }

        void on_message(connection_hdl hdl, server::message_ptr msg) {
        // 接收到消息时的处理
        unsigned int mes = std::stoi(msg->get_payload());
        std::lock_guard<std::mutex> lock(queueMutex);
        mes_queue.push(mes);
        LOG_DEBUG ("Received message: " + msg->get_payload()) ;
    }

    static std::queue<unsigned int>& get_mes_queue(){
        std::lock_guard<std::mutex> lock(get_server()->queueMutex);
        return get_server()->mes_queue;
    }


    void send_mes(connection_hdl hdl,gObj* obj){
        // 创建 JSON 数据
        json j = to_json(*obj);

        // 将 JSON 数据转换为字符串
        std::string json_str = j.dump();

        // 发送 JSON 数据到前端
        m_server.send(hdl, json_str, websocketpp::frame::opcode::text);
    }
    void send_mes(connection_hdl hdl,const std::string& msg){
        // 发送 JSON 数据到前端
        m_server.send(hdl, msg, websocketpp::frame::opcode::text);
    }

    static void send_obj(gObj* pt){
        for (auto &hdl : get_server()->m_connections) {
            get_server()->send_mes(hdl,pt);
        }
    }
    static void send_released_id(int id){
        for (auto &hdl : get_server()->m_connections) {
            json j = json{
                {"id", id},
                {"flags", "released"}
            };
            get_server()->send_mes(hdl,j.dump());
        }
    }
    static void initServer(uint16_t port){
        get_server()->run(port);
    }
    static WebSocketServer* get_server(){
        static WebSocketServer server;
        return &server;
    }
    std::mutex connectMutex;
    static std::condition_variable connect_cv;
    bool connectionStarted = false;
private:
    server m_server;
    std::set<connection_hdl, std::owner_less<connection_hdl>> m_connections;
    //c++的switch只能使用数值类型
    //因此msg的类型信息需要靠前端和后端约定
    //暂时假设这样3个消息
    // 0. 暂停
    // 1. 恢复
    // 2. 退出游戏
    std::mutex queueMutex;
    std::queue<unsigned int> mes_queue;
    
    

};

inline json to_json(const gObj& obj){
    json tmep_j =json {
        {"x", obj.getCrd().get_x()},
        {"y", obj.getCrd().get_y()},
        {"id", obj.getID()},
        {"angle", obj.getAngle().getDegrees()},
        {"type", std::to_string(static_cast<int>(obj.getType()))},
    };
    for (auto &flag : obj.flags) {
        tmep_j["flags"].push_back(flag);
    }
    return tmep_j;
}

