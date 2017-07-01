//
// Created by radimir on 13/05/17.
//

#include "proxy_timer.h"
#include "proxy_server.h"

namespace proxy {
    proxy_timer::proxy_timer(proxy_server *server)
        : epoll_client(std::make_unique<network::timer_descriptor>(), server->get_epoll()), server(server)
    {
        init(true, false);
        network::make_non_blocking(*timer());
    }

    void proxy_timer::on_read()
    {
        timer()->count();
        server->do_cleanup();
    }

    network::timer_descriptor *proxy_timer::timer() {
        return static_cast<network::timer_descriptor *>(get_fd());
    }

    void proxy_timer::set(uint64_t time)
    {
        proxy::log(util::DEBUG) << "timer set to " << time << ".\n";
        timer()->set_time(time);
    }
}
