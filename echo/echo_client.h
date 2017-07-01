//
// Created by radimir on 13.01.16.
//

#ifndef PROXY_ECHO_CLIENT_H
#define PROXY_ECHO_CLIENT_H


#include "../network/epoll.h"
#include "../network/client_socket.h"
#include "../util/composed_buffer.h"
#include "../util/log.h"

namespace echo
{
    static util::log log{"echo"};

    class echo_client : network::epoll_client
    {
    private:
        util::composed_buffer<char> buf;

        void on_read() override;

        void on_write() override;

        network::client_socket *socket() noexcept;

    public:
        echo_client(std::unique_ptr<network::client_socket> &&socket, network::epoll *ep);
    };

} //namespace echo


#endif //PROXY_ECHO_CLIENT_H
