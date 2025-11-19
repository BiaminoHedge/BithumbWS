#include "WsSharedState.h"
#include "WsSession.h"

#include <algorithm>

void WsSharedState::join(std::weak_ptr<WsSession> ws)
{
    sessions_.push_back(std::move(ws));
}

void WsSharedState::leave(WsSession* ws_raw)
{
    sessions_.erase(
        std::remove_if(sessions_.begin(), sessions_.end(),
            [ws_raw](const std::weak_ptr<WsSession>& w){
                auto sp = w.lock();
                return !sp || sp.get() == ws_raw;
            }),
        sessions_.end());
}

void WsSharedState::broadcast(const std::string& message)
{
    std::vector<std::shared_ptr<WsSession>> alive;
    for (auto it = sessions_.begin(); it != sessions_.end();) {
        if (auto sp = it->lock()) {
            alive.push_back(sp);
            ++it;
        } else {
            it = sessions_.erase(it);
        }
    }
    for (auto& s : alive) s->send(message);
}


