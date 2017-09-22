//
// Created by radimir on 11/05/17.
//

#include <cstdint>
#include <string>
#include <csignal>
#include "../network/epoll.h"
#include "proxy_server.h"

network::epoll *running_epoll;

void epoll_stop(int) {
    running_epoll->stop();
}

int main(int argc, char **argv)
{
    uint16_t port_raw = 3338;
    if (argc > 1) {
        port_raw = static_cast<uint16_t >(std::stoi(argv[1]));
    }

    try {
        network::epoll ep{};
        proxy::proxy_server proxy{network::local_endpoint(network::port_t{port_raw, network::HOST}), &ep};

        struct sigaction action;
        memset(&action, 0, sizeof(struct sigaction));
        action.sa_handler = &epoll_stop;
        sigaction(SIGINT, &action, NULL);

        proxy::log(util::INFO) << "Proxy started at " << network::socket_endpoint(*proxy.socket()) << ".\n";

        running_epoll = &ep;
        ep.run();
        running_epoll = nullptr;

    } catch (std::exception const &e) {
        proxy::log(util::ERROR) << "Exception caught: " << e.what() << ".\n";
    }
    proxy::log(util::INFO) << "Proxy closed\n";
}