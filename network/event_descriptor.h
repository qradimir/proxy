//
// Created by radimir on 12/05/17.
//

#ifndef CPP_COURSE_EVENT_DESCRIPTOR_H
#define CPP_COURSE_EVENT_DESCRIPTOR_H


#include "file_descriptor.h"

namespace network {

    class event_descriptor : public file_descriptor
    {
    public:
        event_descriptor();

        size_t read();

        void write(size_t);
    };
}


#endif //CPP_COURSE_EVENT_DESCRIPTOR_H
