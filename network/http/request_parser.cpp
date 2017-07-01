//
// Created by radimir on 11/05/17.
//

#include <iostream>
#include "request_parser.h"
#include "read_util.h"

namespace network { namespace http
{

    request_parser::request_parser(util::composed_buffer<char> *buffer)
            : parser_state(ON_STARTING_LINE)
              , buffer(buffer)
    {
    }

    std::unique_ptr<request> request_parser::try_advance()
    {
        while (true) {
            if (buffer->empty())
                return {};
            switch (parser_state) {
                case ON_STARTING_LINE: {
                    std::unique_ptr<std::string> type_ref = scan_word_until_space(buffer);
                    if (type_ref) {
                        parsed_type = from_string(*type_ref);
                        parser_state = ON_STARTING_LINE_URI;
                        break;
                    }
                    return {};
                }
                case ON_STARTING_LINE_URI: {
                    std::unique_ptr<std::string> uri_ref = scan_word_until_space(buffer);
                    if (uri_ref) {
                        parsed_uri = *uri_ref;
                        parser_state = ON_STARTING_LINE_VERSION;
                        break;
                    }
                    return {};
                }
                case ON_STARTING_LINE_VERSION: {
                    std::unique_ptr<std::string> version_ref = scan_word_until_crlf(buffer);
                    if (version_ref) {
                        parsed_version = *version_ref;
                        parser_state = ON_HEADER_KEY;
                        break;
                    }
                    return {};
                }
                case ON_HEADER_KEY: {
                    if (matches(buffer->begin(), buffer->end(), CRLF)) {
                        buffer->read(buffer->begin() += CRLF.size());
                        /*reset*/ parser_state = ON_STARTING_LINE;
                        return std::make_unique<request>(request_starting_line{parsed_type, parsed_uri, parsed_version},
                                                         std::move(parsed_headers));
                    }
                    std::unique_ptr<std::string> header_key_ref = scan_word_until_colon(buffer);
                    if (header_key_ref) {
                        parsed_key = *header_key_ref;
                        parser_state = ON_HEADER_VALUE;
                        break;
                    }
                    return {};
                }
                case ON_HEADER_VALUE: {
                    std::unique_ptr<std::string> header_value_ref = scan_word_until_crlf(buffer);
                    if (header_value_ref) {
                        parsed_headers.insert(std::make_pair(parsed_key, *header_value_ref));
                        parser_state = ON_HEADER_KEY;
                        break;
                    }
                    return {};
                }
            }
        }
    }
}}