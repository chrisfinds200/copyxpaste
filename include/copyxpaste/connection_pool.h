//
// Created by Chris on 10/11/25.
//

#ifndef COPYXPASTE_CONNECTION_POOL_H
#define COPYXPASTE_CONNECTION_POOL_H
#include <memory>
#include <set>

#include "tcp_connection.h"
#include "asio/steady_timer.hpp"

class tcp_connection;

class connection_pool {
public:
    explicit connection_pool(asio::io_context& io_context) : poll_timer_(io_context), clipboard_("") {};
    void join(const std::shared_ptr<tcp_connection>& connection);
    void disconnect_all();
    void disconnect(const std::shared_ptr<tcp_connection>& connection);
    void fanout_data(const std::string& data, const std::shared_ptr<tcp_connection>& invoker) const;
    void poll_clipboard();
    void stop_poll_clipboard();
    std::set<std::shared_ptr<tcp_connection>>& get_connections() {return connections_;}

private:
    std::set<std::shared_ptr<tcp_connection>> connections_;
    std::string clipboard_;
    asio::steady_timer poll_timer_;
};

#endif //COPYXPASTE_CONNECTION_POOL_H