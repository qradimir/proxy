//
// Created by radimir on 12.01.16.
//

#include <sys/epoll.h>
#include <iostream>
#include "epoll.h"
#include "../util/log.h"
#include "../util/interrupted.h"


const int MAX_EVENT_COUNT = 15;
const int CALLBACK_ON_READ = EPOLLIN;
const int CALLBACK_ON_WRITE = EPOLLOUT;
const int CALLBACK_ON_CLOSE = EPOLLERR | EPOLLRDHUP;

using namespace util;

/*
 *      epoll
 */
namespace network
{
    util::log log_epoll{"epoll"};

    epoll::epoll()
            : file_descriptor(::epoll_create1(0))
    {
    }

    void epoll::add_client(epoll_client *cl)
    {
        log_epoll(DEBUG) << "epoll client (" << cl->fd->raw_fd() << ") added" << "\n";
        epoll_event event;
        event.events = 0;

        if (cl->call_on_read)
            event.events |= CALLBACK_ON_READ;
        if (cl->call_on_write)
            event.events |= CALLBACK_ON_WRITE;

        event.events |= CALLBACK_ON_CLOSE;

        event.data.ptr = cl;
        epoll_ctl(raw_fd(), EPOLL_CTL_ADD, cl->fd->raw_fd(), &event);
    }

    void epoll::modify_client(epoll_client *cl)
    {
        log_epoll(DEBUG) << "epoll client (" << cl->fd->raw_fd() << ") modified" << "\n";
        epoll_event event;
        event.events = 0;

        if (cl->call_on_read)
            event.events |= CALLBACK_ON_READ;
        if (cl->call_on_write)
            event.events |= CALLBACK_ON_WRITE;

        event.events |= CALLBACK_ON_CLOSE;

        event.data.ptr = cl;
        epoll_ctl(raw_fd(), EPOLL_CTL_MOD, cl->fd->raw_fd(), &event);
    }


    void epoll::delete_client(epoll_client *cl)
    {
        log_epoll(DEBUG) << "epoll client (" << cl->fd->raw_fd() << ") deleted" << "\n";
        epoll_ctl(raw_fd(), EPOLL_CTL_DEL, cl->fd->raw_fd(), nullptr);
    }

    void epoll::run()
    {
        running = true;
        epoll_event events[MAX_EVENT_COUNT];
        int n;
        while (running) {
            try {
                n = assert_ret_code(epoll_wait(raw_fd(), events, MAX_EVENT_COUNT, -1));
            } catch (util::interrupted const &e) {
                continue;
            }

            for (int i = 0; i < n; ++i) {
                auto cl = static_cast<epoll_client *>(events[i].data.ptr);
                bool closed = false;
                if (running && events[i].events & CALLBACK_ON_READ) {
                    log_epoll(DEBUG) << "reading from " << cl->fd->raw_fd() << "...\n";
                    try {
                        cl->on_read();
                    } catch (std::exception const &e) {
                        log_epoll(ERROR) << "Error occurred on reading from " << cl->fd->raw_fd() << " : " << e.what() << ".\n";
                        cl->close();
                        closed = true;
                    }
                }
                if (!closed && running && events[i].events & CALLBACK_ON_WRITE) {
                    log_epoll(DEBUG) << "writing to " << cl->fd->raw_fd() << "...\n";
                    try {
                        cl->on_write();
                    } catch (std::exception const &e) {
                        log_epoll(ERROR) << "Error occurred on writing to " << cl->fd->raw_fd() << " : " << e.what() << ".\n";
                        cl->close();
                        closed = true;
                    }
                }
                if (!closed && running && events[i].events & CALLBACK_ON_CLOSE) {
                    log_epoll(DEBUG) << "closing " << cl->fd->raw_fd() << "...\n";
                    cl->close();
                }
            }
        }
    }

    void epoll::stop() noexcept
    {
        running = false;
    }

/*
 *      epoll_client
 */

    epoll_client::epoll_client(std::unique_ptr<file_descriptor> fd, epoll *ep)
            :
            fd(std::move(fd)), ep(ep)
    {
    }

    void epoll_client::init(bool call_on_read, bool call_on_write)
    {
        this->call_on_read = call_on_read;
        this->call_on_write = call_on_write;
        ep->add_client(this);
        open = true;
    }

    void epoll_client::update(bool call_on_read, bool call_on_write)
    {
        if (this->call_on_read == call_on_read && this->call_on_write == call_on_write)
            return;
        this->call_on_read = call_on_read;
        this->call_on_write = call_on_write;
        ep->modify_client(this);
    }

    void epoll_client::close()
    {
        if (open) {
            open = false;
            ep->delete_client(this);
            on_close();
        }
    }

    void epoll_client::on_read() {}

    void epoll_client::on_write() {}

    void epoll_client::on_close() {}

    epoll *epoll_client::get_epoll() noexcept
    {
        return ep;
    }

    file_descriptor *epoll_client::get_fd() noexcept
    {
        return fd.get();
    }
}