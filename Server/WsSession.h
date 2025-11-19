#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class WsSharedState;

class WsSession : public std::enable_shared_from_this<WsSession>
{
public:
    WsSession(tcp::socket socket, std::shared_ptr<WsSharedState> state);
    void run();
    void send(const std::string& message);

private:
    void on_accept(beast::error_code ec);
    void on_read(beast::error_code ec, std::size_t);
    void do_write();
    void on_write(beast::error_code ec, std::size_t);

    websocket::stream<beast::tcp_stream> ws_;
    std::shared_ptr<WsSharedState> state_;
    beast::flat_buffer buffer_;
    std::vector<std::string> queue_;
    bool writing_ {false};
};


