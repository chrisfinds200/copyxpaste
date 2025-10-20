//
// Created by Chris on 10/6/25.
//

#ifndef COPYXPASTE_CXP_CLIENT_H
#define COPYXPASTE_CXP_CLIENT_H
#include <asio/ip/tcp.hpp>
#include <utility>
#include <deque>

#include "clip_message.hpp"

class cxp_client : public std::enable_shared_from_this<cxp_client> {
public:
    typedef std::shared_ptr<cxp_client> pointer;

    static pointer create(asio::io_context& io_context, const std::string& host, const int port) {
        return pointer(new cxp_client(io_context, host, port));
    }


    void connect();
    void disconnect();

private:
    asio::io_context& io_context_;
    asio::ip::tcp::socket socket_;
    asio::steady_timer poll_;
    std::string clipboard_;
    clip_message readbuf_;
    std::string host_;
    std::deque<std::shared_ptr<clip_message>> write_q_;

    int port_;

    void read_data();
    void read_body();
    void send_data(std::string& clip);
    void do_send();
    void start_poll();
    explicit cxp_client(asio::io_context& io_context, std::string host, const int port)
        : io_context_(io_context), socket_(io_context), host_(std::move(host)), port_(port), poll_(io_context), readbuf_({}), clipboard_(""){}
};


#endif //COPYXPASTE_CXP_CLIENT_H