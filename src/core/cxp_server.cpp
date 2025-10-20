//
// Created by Chris on 10/6/25.
//

#include "../../include/copyxpaste/cxp_server.h"

#include <iostream>
#include <thread>
#include <asio/post.hpp>

#include "tcp_connection.h"

void cxp_server::run() {
    std::cout << "CopyXpaste Started!" << std::endl;

    std::thread server_thread([&]{
        connection_pool_.poll_clipboard();
        start_accept();
        io_context_.run();
    });

    std::string user_io;
    while (std::getline(std::cin, user_io)) {
        if (user_io == "exit") {
            asio::post(io_context_, [&]{
                this->stop();
            });
            break;
        }
    }

    server_thread.join();
}

void cxp_server::start_accept() {
    std::shared_ptr<tcp_connection> new_connection = tcp_connection::create(io_context_, connection_pool_);
    acceptor_.async_accept(new_connection->socket(),
        [this, new_connection](const std::error_code& ec) {
            if (!ec) {
                std::cout << "New connection established." << std::endl;
                connection_pool_.join(new_connection);
            } else {
                std::cerr << ec.message() << std::endl;
                return;
            }

            // Prime for next connection
            start_accept();
        });
}

void cxp_server::stop() {
    acceptor_.close();
    connection_pool_.stop_poll_clipboard();
    connection_pool_.disconnect_all();
}