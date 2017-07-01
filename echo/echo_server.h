//
// Created by radimir on 07/05/17.
//

#ifndef PROXY_ECHO_SERVER_H
#define PROXY_ECHO_SERVER_H


#include <set>
#include <memory>
#include "echo_client.h"
#include "../network/server_socket.h"

namespace echo
{
    class echo_server : network::epoll_client
    {
    private:
        std::vector<std::unique_ptr<echo_client>> clients;

        void on_read() override;

    public:

        echo_server(network::port_t port, network::epoll *ep);

        network::server_socket *socket();
    };
}


#endif //PROXY_ECHO_SERVER_H
