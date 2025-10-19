//
// Created by Chris on 10/6/25.
//

#ifndef COPYXPASTE_CXP_SERVER_H
#define COPYXPASTE_CXP_SERVER_H
#include <asio/ip/tcp.hpp>

#include "tcp_connection.h"


class cxp_server {
public:
    cxp_server(const unsigned short port, asio::io_context& io_context) :
        port_(port),
        io_context_(io_context),
        acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
        connection_pool_(io_context) {}

    void run();
    void stop();
    connection_pool& get_connection_pool(){return this->connection_pool_;}

private:
    int port_;
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    connection_pool connection_pool_;

    /**
     * Creates a socket and initiates an asynchronous accept operation to wait for a new connection.
     * once a new connection has been accepted / established. This method
     * starts the new connection and prime the acceptor for future connections.
     */
    void start_accept();
};


#endif //COPYXPASTE_CXP_SERVER_H