//
// Created by radimir on 07/05/17.
//

#ifndef PROXY_SIMPLE_BUFFER_H
#define PROXY_SIMPLE_BUFFER_H

#include <cstddef>
#include <algorithm>

namespace util
{
    template<typename T>
    class simple_buffer
    {
    private:
        size_t begin_pos;
        size_t end_pos;
        size_t capacity;
        T *buf;

    public:

        simple_buffer(size_t capacity = 1024)
                : begin_pos(0), end_pos(0), capacity(capacity), buf(new T[capacity])
        {
        }

        simple_buffer(simple_buffer const &other)
                : begin_pos(other.begin_pos), end_pos(other.end_pos), capacity(other.capacity)
        {
            for (size_t i = begin_pos; i < end_pos; i++) {
                buf[i] = other.buf[i];
            }
        }

        simple_buffer(simple_buffer &&other)
                : begin_pos(other.begin_pos), end_pos(other.end_pos), capacity(other.capacity), buf(nullptr)
        {
            std::swap(buf, other.buf);
        }

        ~simple_buffer()
        {
            if (buf != nullptr) {
                delete[] buf;
            }
        }

        T const *cbegin() const noexcept
        {
            return buf + begin_pos;
        }

        T *begin() noexcept
        {
            return buf + begin_pos;
        }

        T const *cend() const noexcept
        {
            return buf + end_pos;
        }

        T *end() noexcept
        {
            return buf + end_pos;
        }

        void operator=(simple_buffer const &other) {
            begin_pos = other.begin_pos;
            end_pos = other.begin_pos;
            capacity = other.capacity;
            buf = new T[capacity];
            for (size_t i = begin_pos; i < end_pos; i++) {
                buf[i] = other.buf[i];
            }
        }

        void operator=(simple_buffer  &&other) {
            begin_pos = other.begin_pos;
            end_pos = other.begin_pos;
            capacity = other.capacity;
            std::swap(buf, other.buf);
        }


        size_t pop(size_t count) noexcept
        {
            size_t pop_count = std::min(count, end_pos - begin_pos);
            begin_pos += pop_count;
            return pop_count;
        }

        size_t push(size_t count) noexcept
        {
            size_t push_count = std::min(count, capacity - end_pos);
            end_pos += push_count;
            return push_count;
        }

        size_t size() const noexcept
        {
            return end_pos - begin_pos;
        }

        size_t remaining() const noexcept
        {
            return capacity - end_pos;
        }
    };
}

#endif //PROXY_SIMPLE_BUFFER_H
