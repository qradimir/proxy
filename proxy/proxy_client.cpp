//
// Created by radimir on 11/05/17.
//


#include "proxy_client.h"
#include "proxy_client_host.h"
#include "proxy_server.h"

namespace proxy
{

    void proxy_client::on_read()
    {
        active = true;

        util::simple_buffer<char> &buffer = read_buf.back_buffer();
        const char *begin = buffer.end();
        socket()->receive(buffer);
        const char *end = buffer.end();
        std::string msg{begin, end};
        read_buf.adjust_back();
        log(util::DEBUG) << "Request part received '" << util::to_raw(msg) << "'\n";

        if (scanner) {
            if (scanner->try_advance()) {
                scanner = nullptr;
            }
        } else {
            std::unique_ptr<network::http::request> request_ref = parser.try_advance();
            if (request_ref) {
                network::http::request &request = *request_ref;
                request.relativize();

                std::string req = to_string(request);
                log(util::INFO) << "Request parsed:\n" << req;
                std::string host_name = network::http::extract_host(request);

                server->request_resolution(host_name, this);
                close_host();

                std::vector<char> serialized_req{req.cbegin(), req.cend()};
                write_buf.write(serialized_req);

                scanner = network::http::scanner_for(request, &read_buf, &write_buf);
                if (scanner && scanner->try_advance()) {
                    scanner = nullptr;
                }
            }
        }
        update_host();
    }


    void proxy_client::on_write()
    {
        if (host) {
            util::simple_buffer<char> &buffer = host->response_buf.front_buffer();
            socket()->send(buffer);
            host->response_buf.adjust_front();
            update(true, !host->response_buf.empty());
        } else {
            update(true, false);
        }
    }

    void proxy_client::on_close()
    {
        close_host();
        server->drop_client(this);
    }

    network::client_socket *proxy_client::socket() noexcept
    {
        return static_cast<network::client_socket *>(get_fd());
    }

    proxy_client::proxy_client(std::unique_ptr<network::client_socket> &&socket, proxy_server *server)
            : epoll_client(std::move(socket), server->get_epoll()), server(server), read_buf(), parser(&read_buf)
    {
        network::make_non_blocking(*this->socket());
        init(true, false);
    }

    void proxy_client::setup_host(network::ipv4_endpoint endpoint, std::string const &host_name)
    {
        auto host_socket = std::make_unique<network::client_socket>(network::connect(endpoint, true));
        host = std::make_unique<proxy_client_host>(std::move(host_socket), this, host_name);
        proxy::log(util::INFO) << "Client (" << get_fd()->raw_fd() << ") connected to "
                               << host_name << " (" << host->get_fd()->raw_fd() << ").\n";
    }

    std::unique_ptr<network::epoll_client> extract_epoll_client(std::unique_ptr<proxy_client_host> &&host) {
        auto host_ptr = host.release();
        return std::unique_ptr<network::epoll_client>(static_cast<network::epoll_client *>(host_ptr));
    }

    void proxy_client::close_host()
    {
        if (host) {
            proxy::log(util::INFO) << "Client (" << get_fd()->raw_fd() << ") disconnected from "
                                   << host->name << " (" << host->get_fd()->raw_fd() << ").\n";
            host->close();
            write_buf = {};
            get_epoll()->schedule_close(extract_epoll_client(std::move(host)));
        }
    }

    void proxy_client::update_host()
    {
        if (host) {
            host->update(true, !write_buf.empty());
        }
    }
}