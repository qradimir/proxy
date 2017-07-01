//
// Created by radimir on 10/05/17.
//

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>

#include "ipv4.h"
#include "network_error.h"

namespace network
{
    inline uint16_t to_network_byte_order(uint16_t p, byte_order order) noexcept
    {
        if (order == HOST)
            return htons(p);
        else
            return p;
    }

    inline uint16_t from_network_byte_order(uint16_t p, byte_order order) noexcept
    {
        if (order == HOST)
            return ntohs(p);
        else
            return p;
    }

    port_t::port_t(uint16_t port, byte_order order) noexcept
            : p(to_network_byte_order(port, order))
    {
    }

    port_t::operator uint16_t() const noexcept
    {
        return p;
    }

    uint16_t port_t::raw(byte_order order) const noexcept
    {
        return from_network_byte_order(p, order);
    }


    ipv4_address::ipv4_address(uint32_t addr) noexcept
            : addr(addr)
    {
    }

    ipv4_address::operator uint32_t() const noexcept
    {
        return addr;
    }

    ipv4_endpoint::ipv4_endpoint(ipv4_address address, port_t port) noexcept
            : address(address), port(port)
    {
    }

    ipv4_address ipv4_endpoint::get_address() const noexcept
    {
        return address;
    }

    port_t ipv4_endpoint::get_port() const noexcept
    {
        return port;
    }

    ipv4_address any_address() noexcept
    {
        return ipv4_address(INADDR_ANY);
    }

    ipv4_endpoint any_endpoint(ipv4_address address) noexcept
    {
        return ipv4_endpoint(address, port_t{0});
    }

    ipv4_endpoint local_endpoint(port_t port) noexcept
    {
        return ipv4_endpoint(network::any_address(), port);
    }

    std::vector<ipv4_endpoint> get_hosts(std::string const &name)
    {
        addrinfo hints;
        addrinfo *info;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        int code;
        if ((code = getaddrinfo(name.c_str(), "http", &hints, &info)) != 0)
            throw network_error{gai_strerror(code)};

        std::vector<ipv4_endpoint> endpoints{};
        try
        {
            for (addrinfo *p = info; p != nullptr; p = p->ai_next)
            {
                sockaddr_in *address = reinterpret_cast<sockaddr_in *>(p->ai_addr);
                endpoints.emplace_back(address->sin_addr.s_addr, address->sin_port);
            }
        }
        catch (std::exception const &exception)
        {
            freeaddrinfo(info);
            throw exception;
        }

        freeaddrinfo(info);
        return std::move(endpoints);
    }

    std::string to_string(ipv4_address address)
    {
        return inet_ntoa({address});
    }

    std::string to_string(ipv4_endpoint endpoint)
    {
        return network::to_string(endpoint.get_address()) + ":" + std::to_string(endpoint.get_port().raw(HOST));
    }

    std::ostream &operator<<(std::ostream &s, ipv4_address address)
    {
        return s << to_string(address);
    }

    std::ostream &operator<<(std::ostream &s, ipv4_endpoint endpoint)
    {
        return s << to_string(endpoint);
    }
}