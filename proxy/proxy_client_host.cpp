//
// Created by radimir on 11/05/17.
//

#include "proxy_client_host.h"

namespace proxy
{

    void proxy_client_host::on_read()
    {
        client->active = true;
        
        util::simple_buffer<char> &buffer = response_buf.back_buffer();
        socket()->receive(buffer);
        response_buf.adjust_back();

        client->update(true, !response_buf.empty());
    }

    void proxy_client_host::on_write()
    {
        util::simple_buffer<char> &buffer = client->write_buf.front_buffer();
        const char *begin = buffer.begin();
        socket()->send(buffer);
        const char *end = buffer.begin();
        std::string msg{begin, end};
        client->write_buf.adjust_front();
        log(util::DEBUG) << "Request part sent '" << util::to_raw(msg) << "'\n";

        update(true, !client->write_buf.empty());
    }

    network::client_socket *proxy_client_host::socket() noexcept
    {
        return static_cast<network::client_socket *>(get_fd());
    }

    proxy_client_host::proxy_client_host(std::unique_ptr<network::client_socket> &&socket, proxy_client *client, const std::string &name)
        : epoll_client(std::move(socket), client->get_epoll()), client(client), name(name)
    {
        init(true, true);
    }
}