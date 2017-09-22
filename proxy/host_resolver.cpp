//
// Created by radimir on 12/05/17.
//

#include "host_resolver.h"
#include "proxy_server.h"

namespace proxy {

    host_resolver::host_resolver(proxy_server *server)
            : epoll_client(std::make_unique<network::event_descriptor>(), server->get_epoll())
              , executor(10)
              , results()
              , server(server) {
        init(true, false);
    }

    void host_resolver::consume_host(std::string const &host) {
        executor.consume([this, host]() {
            std::vector<network::ipv4_endpoint> endpoints;
            try {
                endpoints = network::get_hosts(host);
            } catch (network::network_error &) {
                endpoints = {};
            }
            results.push(std::make_pair(host, std::move(endpoints)));
            get_notification()->write(1);
        });
    }

    void host_resolver::on_read() {
        size_t ready_n = get_notification()->read();
        for (size_t i = 0; i < ready_n; ++i) {
            resolve_result result = results.pop();
            server->supply_resolution(result.first, result.second);
        }
    }

    network::event_descriptor *host_resolver::get_notification() {
        return static_cast<network::event_descriptor *>(get_fd());
    }
}