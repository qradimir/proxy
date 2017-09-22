//
// Created by radimir on 06/05/17.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include "client_socket.h"
#include "network_error.h"

using namespace util;

namespace network
{

    client_socket::client_socket(int fd) noexcept
            : file_descriptor(fd)
    {
    }

    client_socket::client_socket(client_socket &&other) noexcept
            : file_descriptor(std::move(other))
    {
    }

    size_t client_socket::receive(simple_buffer<char> &buf)
    {

        auto read = static_cast<size_t>(assert_ret_code(::recv(raw_fd(), buf.end(), buf.remaining(), 0)));
        buf.push(read);
        return read;
    }

    size_t client_socket::send(simple_buffer<char> &buf)
    {
        auto sent = static_cast<size_t>(assert_ret_code(::send(raw_fd(), buf.begin(), buf.size(), 0)));
        buf.pop(sent);
        return sent;
    }

    client_socket connect(ipv4_endpoint endpoint, bool non_blocking)
    {
        client_socket socket{assert_ret_code(::socket(AF_INET, SOCK_STREAM, 0))};

        if (non_blocking)
            make_non_blocking(socket);

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = endpoint.get_address();
        address.sin_port = endpoint.get_port();

        int code = ::connect(socket.raw_fd(), reinterpret_cast<sockaddr const *>(&address), sizeof address);
        if (errno != EINPROGRESS)
            assert_ret_code(code);
        return socket;
    }
}