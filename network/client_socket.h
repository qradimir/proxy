//
// Created by radimir on 06/05/17.
//

#ifndef PROXY_CLIENT_SOCKET_H
#define PROXY_CLIENT_SOCKET_H

#include "file_descriptor.h"
#include "../util/simple_buffer.h"

namespace network
{

    class server_socket;

    class client_socket : public file_descriptor
    {
    private:
        client_socket(int fd);

        friend class server_socket;

        friend client_socket connect(ipv4_endpoint endpoint, bool non_blocking);

    public:

        client_socket(client_socket &&);

        size_t receive(util::simple_buffer<char> &buf);

        size_t send(util::simple_buffer<char> &buf);
    };

    client_socket connect(ipv4_endpoint endpoint, bool non_blocking);
}


#endif //PROXY_CLIENT_SOCKET_H
