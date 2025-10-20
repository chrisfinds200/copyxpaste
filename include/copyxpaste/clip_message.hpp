//
// Created by Chris on 10/19/25.
//

#ifndef COPYXPASTE_CLIP_MESSAGE_H
#define COPYXPASTE_CLIP_MESSAGE_H
#include <cstddef>
#include <cstring>

class clip_message {
public:
    enum { header_length = 4 };
    enum { max_body_length = 4096 };

    clip_message() : data_{}, body_length_(0){}

    static std::shared_ptr<clip_message> create(const std::string& payload) {
        std::shared_ptr<clip_message> msg = std::make_shared<clip_message>();
        msg->body_length(payload.size());
        std::memcpy(msg->body(), payload.data(), payload.size());
        msg->encode_header();

        return msg;
    }

    const char* data() const {
        return data_;
    }

    char* data() {
        return data_;
    }

    std::size_t length() const {
        return header_length + body_length_;
    }

    const char* body() const {
        return data_ + header_length;
    }

    char* body() {
        return data_ + header_length;
    }

    std::size_t body_length() const {
        return body_length_;
    }

    void body_length(std::size_t new_length) {
        body_length_ = new_length;
        if (body_length_ > max_body_length)
            body_length_ = max_body_length;
    }

    bool decode_header() {
        char header[header_length + 1] = "";
        std::strncat(header, data_, header_length);
        body_length_ = std::atoi(header);
        if (body_length_ > max_body_length) {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encode_header() {
        char header[header_length + 1] = "";
        std::snprintf(header, sizeof header, "%4d", static_cast<int>(body_length_));
        std::memcpy(data_, header, header_length);
    }

private:
    char data_[header_length + max_body_length];
    std::size_t body_length_;
};





#endif //COPYXPASTE_CLIP_MESSAGE_H