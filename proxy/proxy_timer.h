//
// Created by radimir on 13/05/17.
//

#ifndef CPP_COURSE_PROXY_TIMER_H
#define CPP_COURSE_PROXY_TIMER_H


#include <chrono>
#include "../network/epoll.h"
#include "../network/timer_descriptor.h"

namespace proxy
{

    class proxy_server;

    class proxy_timer : public network::epoll_client
    {
    private:
        proxy_server *server;

        void on_read() override;

        network::timer_descriptor *timer();
    public:

        proxy_timer(proxy_server *server);

        void set(uint64_t time);
    };
}

#endif //CPP_COURSE_PROXY_TIMER_H
