//
// Created by radimir on 11/05/17.
//

#include "request.h"
#include "parse_error.h"

namespace network { namespace http {

    request_starting_line::request_starting_line(std::string const &type, std::string const &uri,
                                                 std::string const &http_version)
            : type(type), uri(uri), http_version(http_version) {
    }

    std::string const &request_starting_line::get_type() const noexcept {
        return type;
    }

    std::string const &request_starting_line::get_uri() const noexcept {
        return uri;
    }

    std::string const &request_starting_line::get_http_version() const noexcept {
        return http_version;
    }

    request::request(request_starting_line const &line, header_map const &headers)
            : starting_line(line), headers(headers) {
    }

    request::request(request_starting_line &&line, header_map &&headers)
            : starting_line(std::move(line)), headers(std::move(headers)) {
    }

    request_starting_line const &request::get_starting_line() const noexcept {
        return starting_line;
    }

    header_map const &request::get_headers() const noexcept {
        return headers;
    }

    header_map &request::get_headers() noexcept {
        return headers;
    }

    void request::relativize() {
        std::string host = extract_host(*this);
        size_t start = starting_line.get_uri().find(host);
        if (start != std::string::npos) {
            std::string new_uri = starting_line.get_uri().substr(start + host.size());
            starting_line = {starting_line.get_type(), new_uri, starting_line.get_http_version()};
        }
    }

    std::string extract_host(request const &req) {
        // TODO take in account uri at starting line if it's absolute
        return req.get_headers().at("Host");
    }

    std::string to_string(request_starting_line const &starting_line) {
        return starting_line.get_type() + " " + starting_line.get_uri() + " " +
               starting_line.get_http_version();
    }

    std::string to_string(request const &req) {
        std::string ret = to_string(req.get_starting_line()) + "\r\n";
        for (auto &&header : req.get_headers()) {
            ret += header.first + ": " + header.second + "\r\n";
        }
        return ret + "\r\n";
    }
}}

