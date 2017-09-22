//
// Created by radimir on 11/05/17.
//

#ifndef CPP_COURSE_PROXY_CLIENT_H
#define CPP_COURSE_PROXY_CLIENT_H


#include <chrono>
#include <list>
#include "../network/epoll.h"
#include "../util/composed_buffer.h"
#include "../network/client_socket.h"
#include "../network/http/request_parser.h"
#include "../util/log.h"
#include "../network/http/msg_scanner.h"

namespace proxy {

    static util::log log{"proxy"};

    class proxy_server;

    class proxy_client_host;

    class proxy_client : public network::epoll_client
    {
    private:
        friend class proxy_client_host;

        proxy_server *server;

        util::composed_buffer<char> read_buf;
        util::composed_buffer<char> write_buf;

        network::http::request_parser parser;
        std::unique_ptr<network::http::msg_scanner> scanner;

        std::unique_ptr<proxy_client_host> host;

        bool got_close = false;

        void on_read() override;

        void on_write() override;

        void on_close() override;

        network::client_socket *socket() noexcept;

        void update_host();

        void drop_host();

    public:
        bool active = true;

        proxy_client(std::unique_ptr<network::client_socket> &&socket, proxy_server *server);

        void setup_host(network::ipv4_endpoint endpoint, std::string const &name);
    };
}



#endif //CPP_COURSE_PROXY_CLIENT_H
