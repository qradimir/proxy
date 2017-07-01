//
// Created by radimir on 11/05/17.
//

#ifndef CPP_COURSE_PARSE_ERROR_H
#define CPP_COURSE_PARSE_ERROR_H


#include <stdexcept>
#include <netinet/in.h>
#include <cstring>

namespace network { namespace http
{
    class parse_error : public std::runtime_error
    {
    public:
        parse_error(std::string const &what)
                : std::runtime_error("Bad Request: " + what) {}

        parse_error()
                : std::runtime_error("Bad Request") {}
    };
}}

#endif //CPP_COURSE_PARSE_ERROR_H
