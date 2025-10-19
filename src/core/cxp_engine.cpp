//
// Created by Chris on 9/30/25.
//
#include "../../include/copyxpaste/cxp_engine.h"

#include <clip.h>
#include <thread>

std::string cxp_engine::curr_clipboard_{};
std::string cxp_engine::last_clipboard_{};

std::string cxp_engine::get_clipboard() {
    if (clip::get_text(curr_clipboard_)) {
        if (curr_clipboard_ != last_clipboard_) {
            last_clipboard_ = curr_clipboard_;
            return curr_clipboard_;
        }
    }
    return "";
}

bool cxp_engine::set_clipboard(const std::string& str) {
    return clip::set_text(str);
}