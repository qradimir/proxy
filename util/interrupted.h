//
// Created by radimir on 12/05/17.
//

#ifndef CPP_COURSE_INTERRUPTED_H
#define CPP_COURSE_INTERRUPTED_H


#include <stdexcept>

namespace util {

    class interrupted : public std::runtime_error {
    public:
        interrupted() : runtime_error("Interrupted") {}
    };
}


#endif //CPP_COURSE_INTERRUPTED_H
