//
// Created by radimir on 13/05/17.
//

#include "timer_descriptor.h"
#include "network_error.h"
#include <sys/timerfd.h>


namespace network
{
    timer_descriptor::timer_descriptor()
            : file_descriptor(assert_ret_code(::timerfd_create(CLOCK_MONOTONIC, 0)))
    {
    }

    uint64_t timer_descriptor::count()
    {
        uint64_t count;
        assert_ret_code(::read(raw_fd(), &count, sizeof(decltype(count))));
        return count;
    }

    void timer_descriptor::set_time(uint64_t duration)
    {
        itimerspec new_value = {};
        itimerspec old_value = {};
        new_value.it_value.tv_sec = duration;
        assert_ret_code(timerfd_settime(raw_fd(), 0, &new_value, &old_value));
    }
}