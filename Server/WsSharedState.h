#pragma once

#include <memory>
#include <string>
#include <vector>

class WsSession;

class WsSharedState {
public:
    void join(std::weak_ptr<WsSession> ws);
    void leave(WsSession* ws_raw);
    void broadcast(const std::string& message);

private:
    std::vector<std::weak_ptr<WsSession>> sessions_;
};