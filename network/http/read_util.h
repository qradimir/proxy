//
// Created by radimir on 12/05/17.
//

#ifndef CPP_COURSE_READ_UTIL_H
#define CPP_COURSE_READ_UTIL_H

#include <string>
#include <memory>
#include "../../util/composed_buffer.h"

namespace network { namespace http
{

    static std::string SPACE = " ";
    static std::string COLON = ": ";
    static std::string CRLF = "\r\n";

    bool matches(util::buffer_iterator<char> scan_it, util::buffer_iterator<char> end_it,
                 std::string const &pattern) noexcept;

    std::unique_ptr<std::string> scan_word_until_space(util::composed_buffer<char> *buffer);

    std::unique_ptr<std::string> scan_word_until_colon(util::composed_buffer<char> *buffer);

    std::unique_ptr<std::string> scan_word_until_crlf(util::composed_buffer<char> *buffer);

}}


#endif //CPP_COURSE_READ_UTIL_H
