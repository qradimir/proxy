//
// Created by radimir on 13.01.16.
//

#include <iostream>
#include "echo_client.h"
#include "../util/log.h"

using namespace network;
using namespace util;

namespace echo
{

    echo_client::echo_client(std::unique_ptr<network::client_socket> &&socket, epoll *ep)
            : epoll_client(std::move(socket), ep), buf()
    {
        make_non_blocking(*this->socket());
        init(true, false);
    }

    void echo_client::on_read()
    {
        simple_buffer<char> &buffer = buf.back_buffer();
        const char *begin = buffer.end();
        socket()->receive(buffer);
        const char *end = buffer.end();
        std::string msg{begin, end};
        buf.adjust_back();

        update(true, true);
        log(INFO) << "got message '" << util::to_raw(msg) << "' from " << network::socket_endpoint(*fd) << "(fd:"
                  << socket()->raw_fd() << ")\n";
    }

    void echo_client::on_write()
    {
        socket()->send(buf.front_buffer());
        buf.adjust_front();
        update(true, !buf.empty());
    }

    network::client_socket *echo_client::socket() noexcept
    {
        return static_cast<network::client_socket *>(fd.get());
    }
}