//
// Created by radimir on 12/05/17.
//

#ifndef CPP_COURSE_SIZED_MSG_SCANNER_H
#define CPP_COURSE_SIZED_MSG_SCANNER_H


#include <memory>
#include "../../util/composed_buffer.h"
#include "request.h"

namespace network { namespace http
{

    class msg_scanner
    {
    protected:
        util::composed_buffer<char> *read_buffer;
        util::composed_buffer<char> *write_buffer;

        msg_scanner(util::composed_buffer<char> *read_buffer, util::composed_buffer<char> *write_buffer);

    public:
        virtual bool try_advance() = 0;
        virtual ~msg_scanner() = default;
    };

    std::unique_ptr<msg_scanner> scanner_for(request const &req,
                                             util::composed_buffer<char> *read_buffer,
                                             util::composed_buffer<char> *write_buffer);
}}


#endif //CPP_COURSE_SIZED_MSG_SCANNER_H
