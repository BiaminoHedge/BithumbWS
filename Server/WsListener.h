#pragma once

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <memory>

namespace net = boost::asio;
namespace beast = boost::beast;
using tcp = net::ip::tcp;

class WsSharedState;
class WsSession;

class WsListener: public std::enable_shared_from_this<WsListener>
{
public:
    WsListener(net::io_context& ioc, const tcp::endpoint& endpoint, std::shared_ptr<WsSharedState> state);
    void run();

private:
    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);

    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    std::shared_ptr<WsSharedState> state_;
};


