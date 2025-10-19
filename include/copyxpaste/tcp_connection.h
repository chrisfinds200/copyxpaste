//
// Created by Chris on 10/5/25.
//

#ifndef COPYXPASTE_TCP_CONNECTION_H
#define COPYXPASTE_TCP_CONNECTION_H
#include <memory>
#include <asio/ip/tcp.hpp>

#include "connection_pool.h"
class connection_pool;

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    static pointer create(asio::io_context& io_context, connection_pool& conn_pool) {
        return pointer(new tcp_connection(io_context, conn_pool));
    }

    asio::ip::tcp::socket& socket() {
        return socket_;
    }

    void start();

    void close();

    std::function<void(std::shared_ptr<tcp_connection>&)> on_close_;

    void handle_write(const std::string& s) {
        handle_write(std::make_shared<std::string>(s));
    }

    void handle_write(const std::shared_ptr<const std::string>& payload);

    void do_write();

    void handle_read();

private:
    asio::ip::tcp::socket socket_;
    std::array<char, 4096> readbuf_;
    connection_pool& connection_pool_;
    std::deque<std::shared_ptr<const std::string>> write_q_;

    explicit tcp_connection(asio::io_context& io_context,
        connection_pool& conn_pool) :
    socket_(io_context),
    connection_pool_(conn_pool),
    on_close_(nullptr){};
};


#endif //COPYXPASTE_TCP_CONNECTION_H