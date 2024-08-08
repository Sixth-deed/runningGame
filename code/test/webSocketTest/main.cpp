#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <sstream>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;
int fib(int a){
    if (a == 1 || a ==2)
        return 1;
    return fib(a - 1) + fib (a -2);
}
void on_open(server* s, websocketpp::connection_hdl hdl) {
    std::cout << "Connection opened" << std::endl;
}

void on_close(server* s, websocketpp::connection_hdl hdl) {
    std::cout << "Connection closed" << std::endl;
}

void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
    int number;
    std::stringstream(msg->get_payload()) >> number;
    int result = fib(number);
    std::stringstream reStream;
    reStream << result;


    s->send(hdl, reStream.str(), msg->get_opcode());
}

int main() {
    server echo_server;
    echo_server.set_open_handler(bind(&on_open, &echo_server, std::placeholders::_1));
    echo_server.set_close_handler(bind(&on_close, &echo_server, std::placeholders::_1));
    echo_server.set_message_handler(bind(&on_message, &echo_server, std::placeholders::_1, std::placeholders::_2));

    echo_server.init_asio();
    echo_server.listen(9002);
    echo_server.start_accept();

    echo_server.run();
}

