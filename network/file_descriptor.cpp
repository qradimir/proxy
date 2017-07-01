//
// Created by radimir on 06/05/17.
//

#include <iostream>
#include <netinet/in.h>
#include "file_descriptor.h"
#include "../util/log.h"
#include "network_error.h"

using namespace util;

namespace network
{

    log log_fd{"file_descriptor"};

    file_descriptor::file_descriptor(int fd)
            : fd(fd)
    {
        log_fd(DEBUG) << "created : " << fd << '\n';
    }

    file_descriptor::~file_descriptor()
    {
        if (fd != -1) {
            log_fd(DEBUG) << "closed : " << fd << '\n';
            close(fd);
        }
    }

    file_descriptor::file_descriptor(file_descriptor &&other)
        : fd(other.fd)
    {
        other.fd = -1;
    }

    void make_non_blocking(file_descriptor const &fd)
    {
        log_fd(DEBUG) << "non-blocking : " << fd << '\n';
        int flags = assert_ret_code(fcntl(fd.raw_fd(), F_GETFL, 0));
        assert_ret_code(fcntl(fd.raw_fd(), F_SETFL, flags | O_NONBLOCK));
    }

    std::ostream &operator<<(std::ostream &s, file_descriptor const &fd)
    {
        return s << fd.raw_fd();
    }

    ipv4_endpoint socket_endpoint(file_descriptor const &fd)
    {
        sockaddr_in addr;
        socklen_t addr_size = sizeof addr;
        assert_ret_code(getsockname(fd.raw_fd(), reinterpret_cast<sockaddr *>(&addr), &addr_size));
        return ipv4_endpoint{addr.sin_addr.s_addr, addr.sin_port};
    }
}
