#include "WsSharedState.h"
#include "WsSession.h"
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace http = boost::beast::http;

WsSession::WsSession(tcp::socket socket, std::shared_ptr<WsSharedState> state)
    : ws_(std::move(socket)), state_(std::move(state)) {}

void WsSession::run()
{
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
    ws_.set_option(websocket::stream_base::decorator([](websocket::response_type& res){
        res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " ws-broadcast-server");
    }));
    ws_.async_accept(
        beast::bind_front_handler(&WsSession::on_accept, shared_from_this()));
}

void WsSession::on_accept(beast::error_code ec)
{
    if (ec) return;
    state_->join(shared_from_this());
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(&WsSession::on_read, shared_from_this()));
}

void WsSession::send(const std::string& message)
{
    queue_.push_back(message);
    if (!writing_) do_write();
}

void WsSession::do_write()
{
    if (queue_.empty()) return;
    writing_ = true;
    ws_.text(true);
    ws_.async_write(boost::asio::buffer(queue_.front()),
        beast::bind_front_handler(&WsSession::on_write, shared_from_this()));
}

void WsSession::on_write(beast::error_code ec, std::size_t)
{
    if (ec) {
        state_->leave(this);
        return;
    }
    queue_.erase(queue_.begin());
    if (!queue_.empty()) do_write(); else writing_ = false;
}

void WsSession::on_read(beast::error_code ec, std::size_t)
{
    if (ec) {
        state_->leave(this);
        return;
    }
    buffer_.consume(buffer_.size());
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(&WsSession::on_read, shared_from_this()));
}


