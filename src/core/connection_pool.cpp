//
// Created by Chris on 10/11/25.
//

#include "../../include/copyxpaste/connection_pool.h"

#include <iostream>
#include "cxp_engine.h"

/**
 * Adds connection to the connection pool and writes clipboard to it.
 * @param connection
 */
void connection_pool::join(const std::shared_ptr<tcp_connection>& connection) {
    connections_.insert(connection);
    std::cout << "Total Connections: " << connections_.size() << std::endl;

    // Start connection, listen to socket for any incoming data
    connection->start();
}

void connection_pool::disconnect_all() {
    const std::vector<std::shared_ptr<tcp_connection>> snapshot(connections_.begin(), connections_.end());
    for (auto& connection : snapshot) {
        connection->close();
        connections_.erase(connection);
    }
}


void connection_pool::disconnect(const std::shared_ptr<tcp_connection>& connection) {
    connections_.erase(connection);
}

void connection_pool::fanout_data(const std::string& data, const std::shared_ptr<tcp_connection>& invoker) const {
    for (const auto& connection : connections_) {
        if (connection == invoker) continue;
        connection->handle_write(data);
    }
}

void connection_pool::poll_clipboard() {
    const std::string clip = cxp_engine::get_clipboard();

    if (!clip.empty() && clip != clipboard_) {
        // update clipboard
        clipboard_ = clip;

        // fan out
        if (!connections_.empty()) {
            this->fanout_data(clip, nullptr);
        }

    }

    poll_timer_.expires_after(std::chrono::milliseconds(500));
    poll_timer_.async_wait([this](const std::error_code& ec){
        if (ec) {
            std::cout << "Error polling clipboard" << std::endl;
            std::cout << ec.message() << std::endl;
            return;
        }
        // Re-arm another poll
        poll_clipboard();
    });
}

void connection_pool::stop_poll_clipboard() {
    poll_timer_.cancel();
}