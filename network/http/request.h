//
// Created by radimir on 11/05/17.
//

#ifndef CPP_COURSE_REQUEST_H
#define CPP_COURSE_REQUEST_H

#include <string>
#include <unordered_map>

namespace network { namespace http
{
    using header_map = std::unordered_map<std::string, std::string>;

    enum request_type : int
    {
        GET, POST
    };

    request_type from_string(std::string const &str);

    class request_starting_line
    {
    private:
        request_type type;
        std::string uri;
        std::string http_version;

    public:
        request_starting_line(request_type type, std::string const &uri, std::string const &http_version);

        request_type get_type() const noexcept;

        std::string const &get_uri() const noexcept;

        std::string const &get_http_version() const noexcept;
    };

    class request
    {
    private:
        request_starting_line starting_line;
        header_map headers;

    public:
        request(request_starting_line const &line, header_map const &headers);

        request(request_starting_line &&line, header_map &&headers);

        request_starting_line const &get_starting_line() const noexcept;

        header_map const &get_headers() const noexcept;

        void relativize();
    };
    
    std::string extract_host(request const &req);

    std::string to_string(request_type type);

    std::string to_string(request_starting_line const &starting_line);

    std::string to_string(request const &req);
}}

#endif //CPP_COURSE_REQUEST_H
