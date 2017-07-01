//
// Created by radimir on 06/05/17.
//

#ifndef PROXY_SOCKET_H
#define PROXY_SOCKET_H

#include <netinet/in.h>
#include "file_descriptor.h"
#include "client_socket.h"
#include "ipv4.h"

namespace network
{
    class server_socket : public file_descriptor
    {
    public:
        server_socket(ipv4_endpoint endpoint);

        client_socket accept();
    };
}


#endif //PROXY_SOCKET_H
