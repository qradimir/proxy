//
// Created by radimir on 06/05/17.
//

#include <sys/socket.h>
#include "server_socket.h"
#include "network_error.h"

using namespace network;

server_socket::server_socket(ipv4_endpoint endpoint)
        : file_descriptor(::socket(AF_INET, SOCK_STREAM, 0))
{
    int enable = 1;
    setsockopt(raw_fd(), SOL_SOCKET, SO_REUSEPORT, &enable, sizeof enable);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = endpoint.get_address();
    addr.sin_port = endpoint.get_port();

    assert_ret_code(
            bind(raw_fd(), reinterpret_cast<sockaddr *>(&addr), sizeof addr)
    );
    assert_ret_code(
            listen(raw_fd(), SOMAXCONN)
    );
}

client_socket server_socket::accept()
{
    return client_socket{assert_ret_code(::accept(raw_fd(), nullptr, nullptr))};
}


