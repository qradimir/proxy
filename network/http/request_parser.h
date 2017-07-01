//
// Created by radimir on 11/05/17.
//

#ifndef CPP_COURSE_REQUEST_PARSER_H
#define CPP_COURSE_REQUEST_PARSER_H

#include <memory>
#include "request.h"
#include "../../util/composed_buffer.h"

namespace network { namespace http
{

    class request_parser
    {
    private:
        enum request_parser_state : int
        {
            ON_STARTING_LINE, ON_STARTING_LINE_URI, ON_STARTING_LINE_VERSION, ON_HEADER_KEY, ON_HEADER_VALUE
        } parser_state;

        util::composed_buffer<char> *buffer;
        request_type parsed_type;
        std::string parsed_uri;
        std::string parsed_version;
        header_map parsed_headers;
        std::string parsed_key;

    public:
        request_parser(util::composed_buffer<char> *buffer);

        std::unique_ptr<request> try_advance();
    };
}}

#endif //CPP_COURSE_REQUEST_PARSER_H
