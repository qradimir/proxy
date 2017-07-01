//
// Created by radimir on 12/05/17.
//

#include "event_descriptor.h"
#include "sys/eventfd.h"
#include "network_error.h"

namespace network {

    event_descriptor::event_descriptor()
        :file_descriptor(::eventfd(0, 0))
    {
    }

    size_t event_descriptor::read()
    {
        size_t res;
        assert_ret_code(::eventfd_read(raw_fd(), &res));
        return res;
    }

    void event_descriptor::write(size_t res)
    {
        assert_ret_code(::eventfd_write(raw_fd(), res));
    }
}