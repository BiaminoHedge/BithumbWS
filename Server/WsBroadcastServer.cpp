#include "WsBroadcastServer.h"

// Оставляем только реализацию WsBroadcastServer
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <iostream>

using namespace std;
namespace net = boost::asio;
using tcp = net::ip::tcp;

#include "WsListener.h"
#include "WsSharedState.h"

WsBroadcastServer::WsBroadcastServer() = default;
WsBroadcastServer::~WsBroadcastServer() { stop(); }

bool WsBroadcastServer::start(const std::string& address, unsigned short port)
{
    if (running_) return true;
    running_ = true;
    state_ = std::make_shared<WsSharedState>();
    ioc_ = std::make_unique<net::io_context>();
    auto addr = net::ip::make_address(address);
    std::make_shared<WsListener>(*ioc_, tcp::endpoint{addr, port}, state_)->run();
    ioc_->run();
    return true;
}

void WsBroadcastServer::stop()
{
    if (!running_) return;
    running_ = false;
    if (ioc_) ioc_->stop();
    ioc_.reset();
    state_.reset();
}

void WsBroadcastServer::broadcast(const std::string& message) const {
    if (state_) state_->broadcast(message);
}


