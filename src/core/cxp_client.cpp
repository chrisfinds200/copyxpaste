//
// Created by Chris on 10/6/25.
//

#include "../../include/copyxpaste/cxp_client.h"

#include <iostream>
#include <thread>
#include <asio/connect.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>
#include <asio/ip/tcp.hpp>

#include "cxp_engine.h"

using asio::ip::tcp;

void cxp_client::connect() {
    // Construct the endpoint based off the host and port provided.
    std::error_code ec;
    const tcp::endpoint endpoint (asio::ip::make_address_v4(host_.c_str(), ec), port_);

    if (ec) {
        std::cerr << ec.message() << std::endl;
        return;
    }

    // Set up socket
    socket_ = tcp::socket(io_context_);

    // Connect
    socket_.connect(endpoint, ec);

    // Read data
    if (!ec && socket_.is_open()) {
        std::thread io_context_thread([&]{
            read_data();
            start_poll();
            io_context_.run();
        });

        std::cout << "Type \"exit\" to quit." << std::endl;
        std::string user_input;
        while (std::getline(std::cin, user_input)) {
            if (user_input == "exit") {
                asio::post(io_context_, [&]{
                    this->disconnect();
                });
                break;
            }
        }
        io_context_thread.join();
    } else {
       std::cerr << "Failed to connect to host " << host_ << ":" << port_ << std::endl;
    }
}

void cxp_client::disconnect() {
    if (socket_.is_open()) {
        std::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_both, ec);

        if (ec == std::errc::not_connected) {
            std::cout << "Connection already closed by peer." << std::endl;
        }

        socket_.close();
    }

    io_context_.stop();
}

void cxp_client::read_data() {
    auto self = shared_from_this();
    asio::async_read(socket_, asio::buffer(readbuf_.data(), clip_message::header_length),
        [self, this](const std::error_code& ec, const std::size_t bytes_transferred) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
                return;
            }

            if (readbuf_.decode_header()) {
                this->read_body();
            }
        });
}

void cxp_client::read_body() {
    auto self = shared_from_this();
    asio::async_read(socket_, asio::buffer(readbuf_.data(), readbuf_.body_length()),
        [self, this](const std::error_code& ec, const std::size_t bytes_transferred) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
                return;
            }

            std::cout << "bytes received: " << bytes_transferred << std::endl;

            // Update clipboard_
            this->clipboard_ = readbuf_.data();

            // Set clipboard
            cxp_engine::set_clipboard(readbuf_.data());

            // Flush read buffer
            this->readbuf_.clear();

            // Re-arm read
            this->read_data();
        });
}


void cxp_client::start_poll() {
    poll_.expires_after(std::chrono::milliseconds(500));
    poll_.async_wait([this](const std::error_code& ec) {
        if (ec) return; // canceled
        const std::string clip = cxp_engine::get_clipboard();

        if (!clip.empty() && clip != clipboard_) {
            clipboard_ = clip;
            send_data(clipboard_);
        }

        // re-arm timer
        start_poll();
    });
}

void cxp_client::send_data(std::string& clip) {
    auto self = shared_from_this();
    const bool idle = write_q_.empty();
    auto payload = clip_message::create(clip);
    write_q_.push_back(std::move(payload));
    if (idle) {
        do_send();
    }
}

void cxp_client::do_send() {
    auto self = shared_from_this();
    asio::async_write(socket_, asio::buffer(write_q_.front()->data(), write_q_.front()->length()),
        [self](const std::error_code& ec, std::size_t bytes_transferred) {
        if (ec) {
            std::cerr << ec.message() << std::endl;
        }

        self->write_q_.pop_front();

        if (!self->write_q_.empty()) {
            self->do_send();
        }
        std::cout << "bytes transferred: " << bytes_transferred << std::endl;
    });
}