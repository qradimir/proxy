//
// Created by radimir on 07/05/17.
//

#include "../network/epoll.h"
#include "echo_server.h"

namespace echo {

    echo_server::echo_server(network::port_t port, network::epoll *ep)
            : epoll_client(std::make_unique<network::server_socket>(network::local_endpoint(port)), ep) {
        make_non_blocking(*socket());
        init(true, false);
    }

    void echo_server::on_read() {
        std::unique_ptr<network::client_socket> client = std::make_unique<network::client_socket>(socket()->accept());
        log(util::INFO) << "Accepted new client from " << network::socket_endpoint(*client) << "(fd:"
                        << client->raw_fd() << ").\n";
        clients.emplace_back(std::make_unique<echo_client>(std::move(client), get_epoll()));
    }

    network::server_socket *echo_server::socket() {
        return static_cast<network::server_socket *>(get_fd());
    }
}