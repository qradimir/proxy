//
// Created by radimir on 12.01.16.
//

#ifndef PROXY_NETWORK_H
#define PROXY_NETWORK_H

#include <exception>
#include <vector>
#include <sys/socket.h>
#include <memory>

#include "network_error.h"
#include "file_descriptor.h"

namespace network
{

    class epoll;

    class epoll_client
    {
    private:
        friend class epoll;

        std::unique_ptr<file_descriptor> fd;
        bool call_on_read, call_on_write;
        bool open = false;

        epoll *ep;

    protected:
        epoll_client(std::unique_ptr<file_descriptor> fd, epoll *ep);

        virtual void on_read();

        virtual void on_write();

        virtual void on_close();

    public:
        void init(bool call_on_read, bool call_on_write);

        void update(bool call_on_read, bool call_on_write);

        void close();

        epoll *get_epoll() noexcept;

        file_descriptor *get_fd() noexcept;
    };

    class epoll : public file_descriptor
    {
        friend class epoll_client;

        void add_client(epoll_client *cl);

        void modify_client(epoll_client *cl);

        void delete_client(epoll_client *cl);

        bool running = false;
    public:
        epoll();

        void run();

        void stop() noexcept;
    };

} //namespace network

#endif //PROXY_NETWORK_H
