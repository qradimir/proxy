//
// Created by radimir on 14.01.16.
//

#ifndef PROXY_UTIL_H
#define PROXY_UTIL_H

#include <unistd.h>
#include <stddef.h>
#include <exception>
#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include "../util/interrupted.h"

namespace network
{

    class network_error : public std::runtime_error
    {
    public:
        network_error(std::string const &what)
                : runtime_error(what)
        {
        }
    };

    template<typename T>
    T assert_ret_code(T ret_code)
    {
        if (ret_code == -1) {
            if (errno == EINTR)
                throw util::interrupted{};
            throw network_error(::strerror(errno));
        }
        return ret_code;
    }
}

#endif //PROXY_UTIL_H
