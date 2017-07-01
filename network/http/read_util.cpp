//
// Created by radimir on 12/05/17.
//

#include "read_util.h"
#include "parse_error.h"

namespace network { namespace http
{

    bool matches(util::buffer_iterator<char> scan_it, util::buffer_iterator<char> end_it,
                        std::string const &pattern) noexcept
    {
        std::string::const_iterator pattern_it = pattern.begin();
        while (pattern_it != pattern.end()) {
            if (scan_it == end_it)
                return false;
            if (*scan_it != *pattern_it)
                return false;

            ++pattern_it;
            ++scan_it;
        }
        return true;
    }

    std::unique_ptr<std::string> scan_word_until_space(util::composed_buffer<char> *buffer)
    {
        for (auto scan_it = buffer->begin(); scan_it != buffer->end(); ++scan_it) {
            if (matches(scan_it, buffer->end(), CRLF)) {
                throw parse_error{"Found - CRLF, Expected - SPACE"};
            }
            if (matches(scan_it, buffer->end(), SPACE)) {
                std::vector<char> res = buffer->read(scan_it);
                buffer->read(scan_it += SPACE.size());
                return std::make_unique<std::string>(res.begin(), res.end());
            }
        }
        return {};
    }

    std::unique_ptr<std::string> scan_word_until_colon(util::composed_buffer<char> *buffer)
    {
        for (auto scan_it = buffer->begin(); scan_it != buffer->end(); ++scan_it) {
            if (matches(scan_it, buffer->end(), CRLF)) {
                throw parse_error{"Found - CRLF, Expected - COLON"};
            }
            if (matches(scan_it, buffer->end(), COLON)) {
                std::vector<char> res = buffer->read(scan_it);
                buffer->read(scan_it += COLON.size());
                return std::make_unique<std::string>(res.begin(), res.end());
            }
        }
        return {};
    }

    std::unique_ptr<std::string> scan_word_until_crlf(util::composed_buffer<char> *buffer)
    {

        for (auto scan_it = buffer->begin(); scan_it != buffer->end(); ++scan_it) {
            if (matches(scan_it, buffer->end(), CRLF)) {
                std::vector<char> res = buffer->read(scan_it);
                buffer->read(scan_it += CRLF.size());
                return std::make_unique<std::string>(res.begin(), res.end());
            }
        }
        return {};
    }

}}