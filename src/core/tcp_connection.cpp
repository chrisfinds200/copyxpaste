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
    asio::async_read(socket_, asio::buffer(readbuf_.data(), clip_message::header_length),
        [self, this](const std::error_code& ec, const std::size_t bytes_transferred) {
            if (ec) {
                std::cout << ec.message() << std::endl;
                return;
            }

            if (readbuf_.decode_header()) {
                handle_read_body();
            }
        });
}

void tcp_connection::handle_read_body() {
    auto self = shared_from_this();
    asio::async_read(socket_, asio::buffer(readbuf_.data(), readbuf_.body_length()),
        [self, this](const std::error_code& ec, const std::size_t bytes_transferred) {
            if (ec) {
                std::cout << ec.message() << std::endl;
                return;
            }

            std::cout << "bytes received: " << bytes_transferred + 4 << std::endl;

            // Update clipboard to avoid sending duplicate
            this->connection_pool_.set_clipboard_(self->readbuf_.data());

            // Update clipboard, this is going to trigger poll hence we need to update clipboard_ before
            cxp_engine::set_clipboard(self->readbuf_.data());

            // Finally, broadcast clipboard
            self->connection_pool_.fanout_data(self->readbuf_.data(), self);

            // Clear buffer
            self->readbuf_.clear();

            // Re-arm read
            handle_read();
        });
}


void tcp_connection::handle_write(const std::shared_ptr<clip_message>& message) {
    const bool idle = write_q_.empty();
    write_q_.push_back(message);
    if (idle) {
        do_write();
    }
}


void tcp_connection::do_write() {
    auto self = shared_from_this();
    const auto message = write_q_.front();
    asio::async_write(socket_, asio::buffer(message->data(), message->length()),
        [self](const std::error_code& ec, const std::size_t bytes_transferred) {
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

