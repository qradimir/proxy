//
// Created by radimir on 11/05/17.
//

#ifndef CPP_COURSE_PROXY_CLIENT_HOST_H
#define CPP_COURSE_PROXY_CLIENT_HOST_H


#include "../network/epoll.h"
#include "../util/composed_buffer.h"
#include "../network/client_socket.h"
#include "proxy_client.h"


namespace proxy{

    class proxy_client;

    class proxy_client_host : public network::epoll_client
    {
    private:
        friend class proxy_client;

        proxy_client *client;
        std::string name;
        util::composed_buffer<char> response_buf;

        void on_read() override;

        void on_write() override;

        network::client_socket *socket() noexcept;

    public:
        proxy_client_host(std::unique_ptr<network::client_socket> &&socket, proxy_client *client, const std::string &name);
    };
}


#endif //CPP_COURSE_PROXY_CLIENT_HOST_H
