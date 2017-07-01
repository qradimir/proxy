#include <iostream>
#include "echo_client.h"
#include "echo_server.h"

int main(int argc, char** argv)
{
    uint16_t port_raw = 3338;
    if (argc > 1) {
        port_raw = static_cast<uint16_t >(std::stoi(argv[1]));
    }

    try {
        network::epoll ep{};
        echo::echo_server server_ep{network::port_t{port_raw, network::HOST}, &ep};

        echo::log(util::INFO) << "Echo-server started at " << network::socket_endpoint(*server_ep.socket()) << ".\n";

        ep.run();
    } catch (std::exception const &e) {
        echo::log(util::ERROR) << "Exception caught: " << e.what() << ". Echo-server closed.\n";
    }
}