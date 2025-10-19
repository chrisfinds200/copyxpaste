//
// Created by Chris on 10/5/25.
//
#include "../../include/copyxpaste/tcp_connection.h"

#include <iostream>
#include <asio/read.hpp>
#include <asio/write.hpp>
#include <asio/ip/tcp.hpp>

#include "cxp_engine.h"

void tcp_connection::start() {
    handle_read();
}

void tcp_connection::close() {
    if (!socket_.is_open()) return;
    std::error_code ec;
    socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    if (ec) {
        if (ec == std::errc::not_connected) {
            std::cout << "Connection already closed by peer." << std::endl;
            return;
        }
        std::cout << ec.message() << std::endl;
    }
    socket_.close();
}

void tcp_connection::handle_read() {
    auto self = shared_from_this();
    socket_.async_read_some(asio::buffer(readbuf_),
        [self](const std::error_code& ec, const std::size_t bytes_transferred) {
            if (ec) {
                std::cout << ec.message() << std::endl;
                return;
            }

            std::cout << "bytes received: " << bytes_transferred << std::endl;
            self->connection_pool_.fanout_data(self->readbuf_.data(), self);

            // Re-arm read
            self->handle_read();
        });
}

void tcp_connection::handle_write(const std::shared_ptr<const std::string>& payload) {
    const bool idle = write_q_.empty();
    write_q_.push_back(payload);
    if (idle) {
        do_write();
    }
}


void tcp_connection::do_write() {
    auto self = shared_from_this();
    const auto data = write_q_.front();
    asio::async_write(socket_, asio::buffer(*data), [self](const std::error_code& ec, std::size_t bytes_transferred) {
        if (ec) {
            std::cerr << ec.message() << std::endl;
            return;
        }
        self->write_q_.pop_front();
        if (!self->write_q_.empty()) {
            self->do_write();
            std::cout << "bytes transferred: " << bytes_transferred << std::endl;
        }
    });
}

