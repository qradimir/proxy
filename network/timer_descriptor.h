//
// Created by radimir on 13/05/17.
//

#ifndef CPP_COURSE_TIMER_DESCRIPTOR_H
#define CPP_COURSE_TIMER_DESCRIPTOR_H


#include "file_descriptor.h"

namespace network
{

    class timer_descriptor : public file_descriptor
    {
    public:
        timer_descriptor();

        uint64_t count();

        void set_time(uint64_t duration);
    };

}

#endif //CPP_COURSE_TIMER_DESCRIPTOR_H
