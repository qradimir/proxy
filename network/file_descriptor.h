//
// Created by radimir on 06/05/17.
//

#ifndef PROXY_FILE_DESCRIPTOR_H
#define PROXY_FILE_DESCRIPTOR_H

#include <fcntl.h>
#include <ostream>
#include "ipv4.h"

namespace network
{

    class file_descriptor
    {
    private:
        int fd;

    public:
        explicit file_descriptor(int fd);

        file_descriptor(file_descriptor const &other) = delete;

        file_descriptor(file_descriptor &&other);

        ~file_descriptor() noexcept;

        inline int raw_fd() const
        {
            return fd;
        }
    };

    void make_non_blocking(file_descriptor const &fd);

    ipv4_endpoint socket_endpoint(file_descriptor const &fd);

    std::ostream& operator<<(std::ostream &s, file_descriptor const &fd);
}

#endif //PROXY_FILE_DESCRIPTOR_H
