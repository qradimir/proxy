//
// Created by radimir on 13/05/17.
//

#include "log.h"

namespace util {

    std::string to_raw(std::string const &str) {
        std::string ret{};
        for (size_t i = 0; i < str.size(); ++i) {
            const char &ch = str[i];
            switch (ch) {
                case '\n':
                    ret.append("\\n");
                    break;
                case '\r':
                    ret.append("\\r");
                    break;
                case '\t':
                    ret.append("\\t");
                    break;
                default:
                    if (ch < 0x20) {
                        ret.append("\\").append(std::to_string(static_cast<unsigned char>(ch)));
                    } else {
                        char cstr[]{ch, '\0'};
                        ret.append(cstr);
                    }
            }
        }
        return ret;
    }


    log::log(std::string name, std::ostream *backed_stream)
            : name(name), stream(backed_stream) {
    }

    log &log::operator()(log_level level) {
        *stream << level_presentation(level) << " " << name << " -- ";
        return *this;
    }

    std::string log::level_presentation(log_level level) {
        switch (level) {
            case DEBUG:
                return "[ DEBUG ]";
            case INFO :
                return "[ INFO  ]";
            case ERROR:
                return "[ ERROR ]";
        }
        return "";
    }
}
