//
// Created by radimir on 11/05/17.
//

#ifndef CPP_COURSE_PROXY_SERVER_H
#define CPP_COURSE_PROXY_SERVER_H

#include <list>
#include <set>
#include <chrono>
#include "../network/epoll.h"
#include "../network/server_socket.h"

#include "proxy_client.h"
#include "proxy_client_host.h"
#include "proxy_timer.h"
#include "host_resolver.h"


namespace proxy
{

    class proxy_client;

    class proxy_server : public network::epoll_client
    {
    private:
        std::unordered_map<int ,std::unique_ptr<proxy_client>> clients;

        std::unordered_multimap<std::string, int> resolve_waiters;
        host_resolver resolver;

        proxy_timer timer;

        void on_read() override;

    public:
        proxy_server(network::ipv4_endpoint endpoint, network::epoll *ep);

        void drop_client(int client_fd);

        void do_cleanup();

        void request_resolution(std::string const &, proxy_client *);

        void supply_resolution(std::string const &, std::vector<network::ipv4_endpoint>);

        network::server_socket *socket();
    };
}

#endif //CPP_COURSE_PROXY_SERVER_H
