#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;      // from <boost/beast.hpp>
namespace http = boost::beast::http; // from <boost/beast/http.hpp>
namespace websocket = boost::beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;         // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;    // from <boost/asio/ip/tcp.hpp>

void on_message(websocket::stream<tcp::socket>& ws) {
    beast::flat_buffer buffer;
    ws.read(buffer);
    std::cout << "Received message: " << beast::make_printable(buffer.data()) << std::endl;
}

int main() {
    try {
        // Set up an I/O context and resolver
        net::io_context ioc;
        tcp::resolver resolver(ioc);
        websocket::stream<tcp::socket> ws(ioc);

        // Resolve the domain name and connect
        auto const results = resolver.resolve("localhost", "8080");
        net::connect(ws.next_layer(), results.begin(), results.end());

        // Perform WebSocket handshake
        ws.handshake("echo.websocket.org", "/");

        // Send a message
        ws.write(net::buffer(std::string("Hello, WebSocket!")));

        // Receive a message
        on_message(ws);

        // Close the WebSocket connection
        ws.close(websocket::close_code::normal);
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

