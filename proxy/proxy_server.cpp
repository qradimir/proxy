//
// Created by radimir on 11/05/17.
//

#include "proxy_server.h"

namespace proxy
{
    uint64_t const DELAY = 15;

    proxy_server::proxy_server(network::ipv4_endpoint endpoint, network::epoll *ep)
            : epoll_client(std::make_unique<network::server_socket>(endpoint), ep), resolver(this), timer(this)
    {
        network::make_non_blocking(*socket());
        init(true, false);
    }

    void proxy_server::on_read()
    {
        if (clients.size() == 0) {
            timer.set(DELAY);
        }
        auto client = std::make_unique<network::client_socket>(socket()->accept());
        int fd = client->raw_fd();
        clients.emplace(fd, std::make_unique<proxy_client>(std::move(client), this));
    }

    std::unique_ptr<network::epoll_client> extract_epoll_client(std::unique_ptr<proxy_client> &&host) {
        auto host_ptr = host.release();
        return std::unique_ptr<network::epoll_client>(static_cast<network::epoll_client *>(host_ptr));
    }

    void proxy_server::drop_client(proxy_client *client)
    {
        auto client_it = clients.find(client->get_fd()->raw_fd());

        if (client_it != clients.end()) {
            get_epoll()->schedule_close(extract_epoll_client(std::move(client_it->second)));
            clients.erase(client_it);
        }
    }

    void proxy_server::supply_resolution(std::string const &host, std::vector<network::ipv4_endpoint> endpoints)
    {
        auto waiters = resolve_waiters.equal_range(host);
        if (waiters.first != waiters.second) {
            if (endpoints.size() == 0) {
                proxy::log(util::DEBUG) << "No resolution for host: " + host;
                for (auto it = waiters.first; it != waiters.second; ++it) {
                    auto client_it = clients.find(it->second);
                    if (client_it != clients.end()) {
                        client_it->second->close();
                    }
                }
            } else {
                network::ipv4_endpoint endpoint = endpoints.front();
                for (auto it = waiters.first; it != waiters.second; ++it) {
                    auto client_it = clients.find(it->second);
                    if (client_it != clients.end()) {
                        client_it->second->setup_host(endpoint, host);
                    }
                }
            }

            resolve_waiters.erase(waiters.first, waiters.second);
        } else {
            proxy::log(util::DEBUG) << "No waiters for resolved host: " + host;
        }
    }

    network::server_socket *proxy_server::socket()
    {
        return static_cast<network::server_socket *>(get_fd());
    }

    void proxy_server::request_resolution(std::string const &host, proxy_client *client)
    {
        if (!resolve_waiters.count(host)) {
            resolver.consume_host(host);
        }
        resolve_waiters.emplace(host, client->get_fd()->raw_fd());
    }

    void proxy_server::do_cleanup()
    {
        proxy::log(util::INFO) << "Clean-up session.\n";
        std::vector<proxy_client *> client_refs{};
        client_refs.reserve(clients.size());
        for (auto &client : clients) {
            client_refs.push_back(client.second.get());
        }
        size_t active_count = 0;
        for (proxy_client *ref : client_refs) {
            if (ref->active) {
                active_count++;
                ref->active = false;
            } else {
                ref->close();
            }
        }
        proxy::log(util::INFO) << "Active clients: " << active_count << ".\n";
        if (active_count) {
            timer.set(DELAY);
        } else {
            timer.set(0);
        }
    }
}


