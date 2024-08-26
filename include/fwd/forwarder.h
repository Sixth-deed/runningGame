//add your code here

//定义sendpacktype类
//覆盖对象坐标，对象id，对象角度，对象类型以及flags这几项

//gobj.h将信号传递到gameMain.cpp

#include "gObj.h"
#include "GameMain.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <set>
#include <string>
#include <functional>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using json = nlohmann::json;

template<typename T>
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
        m_server.set_message_handler([this](connection_hdl hdl,server::message_ptr msg,const T& some_T){
            this->on_message(hdl,msg,some_T);
        });
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
        std::cout << "Connection opened" << std::endl;
        m_connections.insert(hdl);
    }

    void on_close(connection_hdl hdl) {
        // 连接关闭时的处理
        std::cout << "Connection closed" << std::endl;
        m_connections.erase(hdl);
    }

    void on_message(connection_hdl hdl, server::message_ptr msg,const T& some_T) {
        // 接收到消息时的处理
        std::cout << "Received message: " << msg->get_payload() << std::endl;

        // 创建 JSON 数据
        json j;
        to_json(j,some_T);

        // 将 JSON 数据转换为字符串
        std::string json_str = j.dump();

        // 发送 JSON 数据到前端
        m_server.send(hdl, json_str, websocketpp::frame::opcode::text);
    }

private:
    server m_server;
    std::set<connection_hdl, std::owner_less<connection_hdl>> m_connections;
};

void to_json(json& j,gObj& obj){
    j = json{
        {"crdinate_x", obj.getCrd().get_x()},
        {"crdinate_y", obj.getCrd().get_y()},
        {"id", obj.getID()},
        {"angle_deg", obj.getAngle().getDegrees()},
        {"angle_rad", obj.getAngle().getRadians()},
        {"type", std::to_string(static_cast<int>(obj.getType()))},
        {"flags",obj.flags}
    };
}

//add other to_json functions...


int main() {
    WebSocketServer<gObj> server;
    server.run(9002); // 监听 9002 端口
    return 0;
}