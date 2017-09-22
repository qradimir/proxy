//
// Created by radimir on 07/05/17.
//

#ifndef PROXY_LOG_H
#define PROXY_LOG_H

#include <string>
#include <iostream>

namespace util {

    enum log_level : int {
        DEBUG, INFO, ERROR
    };

    class log {
    public:
        log(std::string name, std::ostream *backed_stream = &std::cout);

        log &operator()(log_level level);

        template<typename T>
        log &operator<<(const T &t) {
            *stream << t;
            return *this;
        }

    private:
        std::string name;
        std::ostream *stream;

        std::string level_presentation(log_level level);
    };

    std::string to_raw(std::string const &str);
}
#endif //PROXY_LOG_H
