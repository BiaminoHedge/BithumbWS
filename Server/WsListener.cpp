#include "WsListener.h"
#include "WsSharedState.h"
#include "WsSession.h"

#include <boost/beast/core.hpp>

namespace beast = boost::beast;

WsListener::WsListener(net::io_context& ioc, const tcp::endpoint& endpoint, std::shared_ptr<WsSharedState> state)
    : ioc_(ioc), acceptor_(ioc.get_executor()), state_(std::move(state))
{
    beast::error_code ec;
    acceptor_.open(endpoint.protocol(), ec);
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    acceptor_.bind(endpoint, ec);
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
}

void WsListener::run() { do_accept(); }

void WsListener::do_accept()
{
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(&WsListener::on_accept, shared_from_this()));
}

void WsListener::on_accept(beast::error_code ec, tcp::socket socket)
{
    if (!ec) std::make_shared<WsSession>(std::move(socket), state_)->run();
    do_accept();
}


