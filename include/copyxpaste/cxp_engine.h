//
// Created by Chris on 9/30/25.
//

#ifndef COPYXPASTE_SERVICE_H
#define COPYXPASTE_SERVICE_H
#include <string>


class cxp_engine {
public:
    static std::string get_clipboard();
    static bool set_clipboard(const std::string& str);

private:
    static std::string curr_clipboard_;
    static std::string last_clipboard_;
};


#endif //COPYXPASTE_SERVICE_H