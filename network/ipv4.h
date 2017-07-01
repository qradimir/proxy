//
// Created by radimir on 10/05/17.
//

#ifndef PROXY_IPV4_H
#define PROXY_IPV4_H

#include <cstdint>
#include <string>
#include <vector>

namespace network
{

    class ipv4_address
    {
    private:
        uint32_t addr;
    public:
        ipv4_address(uint32_t addr) noexcept;

        operator uint32_t() const noexcept;

    };

    enum byte_order : bool
    {
        NETWORK,
        HOST
    };

    class port_t
    {
    private:
        uint16_t p;

    public:
        port_t(uint16_t port, byte_order order = NETWORK) noexcept;

        operator uint16_t() const noexcept;

        uint16_t raw(byte_order order = NETWORK) const noexcept;
    };

    class ipv4_endpoint
    {
    private:
        ipv4_address address;
        port_t port;
    public:
        ipv4_endpoint(ipv4_address address, port_t port) noexcept;

        ipv4_address get_address() const noexcept;

        port_t get_port() const noexcept;
    };


    ipv4_address any_address() noexcept;

    ipv4_endpoint any_endpoint(ipv4_address address) noexcept;

    ipv4_endpoint local_endpoint(port_t port) noexcept;

    std::vector<ipv4_endpoint> get_hosts(std::string const &host_name);

    std::string to_string(ipv4_address address);

    std::string to_string(ipv4_endpoint endpoint);

    std::ostream &operator<<(std::ostream &s, ipv4_address address);

    std::ostream &operator<<(std::ostream &s, ipv4_endpoint endpoint);

}
#endif //PROXY_IPV4_H
