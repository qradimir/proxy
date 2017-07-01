//
// Created by radimir on 12/05/17.
//

#ifndef CPP_COURSE_HOST_RESOLVER_H
#define CPP_COURSE_HOST_RESOLVER_H


#include "../util/executor.h"
#include "../network/ipv4.h"
#include "../network/event_descriptor.h"
#include "../network/epoll.h"

namespace proxy
{

    class proxy_server;

    class host_resolver : network::epoll_client
    {
    private:
        using resolve_result = std::pair<std::string, std::vector<network::ipv4_endpoint>>;
        util::executor executor;
        util::blocking_queue<resolve_result> results;
        proxy_server *server;

        network::event_descriptor *get_notification();

    public:

        host_resolver(proxy_server *server);

        void consume_host(std::string const &host);

    private:
        void on_read() override;
    };

}


#endif //CPP_COURSE_HOST_RESOLVER_H
