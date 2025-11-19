#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>

namespace net = boost::asio;

class WsSharedState;

// Высокоуровневый сервер: старт/стоп и broadcast
class WsBroadcastServer
{
public:
    WsBroadcastServer();
    ~WsBroadcastServer();

    bool start(const std::string& address, unsigned short port);
    void stop();
    void broadcast(const std::string& message) const;

private:
    bool running_ {false};
    std::unique_ptr<net::io_context> ioc_;
    std::shared_ptr<WsSharedState> state_;
};


