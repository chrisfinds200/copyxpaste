#include <iostream>
#include <asio.hpp>

#include "cxp_client.h"
#include "cxp_server.h"
#include "cxp_engine.h"

int main(const int argc, char *argv[]) {
    if (argc <= 1) {
        std::cerr << "Usage: copyxpaste" << " [options]" << std::endl;
        return -1;
    }

    asio::io_context io_context;
    if (std::string_view(argv[1]) == "-c") {
        if (argc != 5) {
            std::cerr << "Usage: copyxpaste" << " -c [host] -p [port]" << std::endl;
            return -1;
        }

        // Boot up client mode
        const auto client = cxp_client::create(io_context, argv[2], std::stoi(argv[4]));
        client->connect();
    } else if (std::string_view(argv[1]) == "-h") {
        if (argc != 3) {
            std::cerr << "Usage: copyxpaste" << " -h [port]" << std::endl;
            return -1;
        }

        cxp_server server(std::stoi(argv[2]), io_context);
        server.run();
    }

    return 0;
}