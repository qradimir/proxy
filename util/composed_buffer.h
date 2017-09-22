#ifndef PROXY_COMPOSED_BUFFER_H
#define PROXY_COMPOSED_BUFFER_H

#include <deque>
#include <string>
#include <vector>
#include "simple_buffer.h"

namespace util {
    template<typename T>
    class composed_buffer;

    template<typename T>
    class buffer_iterator {
    private:
        friend class composed_buffer<T>;

        using buffer_t = composed_buffer<T>;
        using iterator = buffer_iterator<T>;
        using sb_iterator = typename std::deque<util::simple_buffer<T>>::const_iterator;
        using t_iterator = T const *;
        using ref = T const &;

        sb_iterator sb_it;
        t_iterator t_it;

        buffer_iterator(sb_iterator const &sb_it, t_iterator const &t_it) noexcept
                : sb_it(sb_it), t_it(t_it) {
        }

    public:

        buffer_iterator(iterator const &other) noexcept
                : buffer_iterator(other.sb_it, other.t_it) {
        }

        bool operator==(iterator const &other) const noexcept {
            return sb_it == other.sb_it && t_it == other.t_it;
        }

        bool operator!=(iterator const &other) const noexcept {
            return !(*this == other);
        }

        iterator &operator=(iterator const &other) noexcept {
            sb_it = other.sb_it;
            t_it = other.t_it;
            return *this;
        }

        iterator &operator++() noexcept {
            t_it = force_t_it();
            t_it++;
            if (t_it == sb_it->cend()) {
                sb_it++;
                t_it = nullptr;
            }
            return *this;
        }

        iterator &operator+=(unsigned long d) noexcept {
            while (true) {
                if (d == 0)
                    return *this;
                else
                    t_it = force_t_it();
                if (static_cast<unsigned long>(sb_it->cend() - t_it) <= d) {
                    d -= (sb_it->cend() - t_it);
                    sb_it++;
                    t_it = nullptr;
                } else {
                    t_it += d;
                    return *this;
                }
            }
        }

        unsigned long operator-(iterator other) const noexcept {
            unsigned long d = 0;
            while (true) {
                if (sb_it != other.sb_it) {
                    other.t_it = other.force_t_it();
                    d += other.sb_it->cend() - other.t_it;
                    other.sb_it++;
                    other.t_it = nullptr;
                } else {
                    if (t_it == nullptr && other.t_it == nullptr) {
                        return d;
                    }
                    d += force_t_it() - other.force_t_it();
                    return d;
                }
            }
        }

        ref operator*() const noexcept {
            return *force_t_it();
        }

        t_iterator operator->() const noexcept {
            return force_t_it();
        }

        ref operator[](unsigned long d) const noexcept {
            iterator tmp = *this;
            tmp += d;
            return *tmp;
        }

    private:
        t_iterator force_t_it() const noexcept {
            if (t_it == nullptr) {
                return sb_it->cbegin();
            }
            return t_it;
        }
    };

    template<typename T>
    class composed_buffer {
    private:
        friend class buffer_iterator<T>;

        using buffer_ref = simple_buffer <T> &;
        using iterator   = buffer_iterator<T>;
    public:
        composed_buffer()
                : buffers() {
        }

        composed_buffer(composed_buffer const &other)
                : buffers(buffers) {
        }

        composed_buffer(composed_buffer &&other)
                : buffers(std::move(other.buffers)) {
        }


        bool empty() const noexcept {
            return buffers.size() == 0;
        }

        iterator begin() const {
            return iterator{buffers.cbegin(), nullptr};
        }

        iterator end() const {
            return iterator{buffers.cend(), nullptr};
        }

        void operator=(composed_buffer const &other) {
            buffers = other.buffers;
        }

        void operator=(composed_buffer const &&other) {
            buffers = std::move(other.buffers);
        }

        std::vector<T> read(iterator const &until) {
            std::vector<T> ret = {};

            if (until.sb_it == buffers.end()) {
                while (!buffers.empty()) {
                    buffer_ref front = buffers.front();
                    ret.insert(ret.end(), front.begin(), front.end());
                    buffers.pop_front();
                }
            } else {
                simple_buffer<T> const *until_sb_ptr = &*(until.sb_it);
                while (&buffers.front() != until_sb_ptr) {
                    buffer_ref front = buffers.front();
                    ret.insert(ret.end(), front.begin(), front.end());
                    buffers.pop_front();
                }
                ret.insert(ret.end(), buffers.front().cbegin(), until.force_t_it());
                buffers.front().pop(until.force_t_it() - buffers.front().begin());
            }
            return std::move(ret);
        }

        void write(std::vector<T> const &vec) {
            auto begin = vec.begin();
            auto len = vec.size();

            while (true) {
                buffer_ref back = back_buffer();
                if (back.remaining() > len) {
                    write_impl(begin, back, len);
                    return;
                } else {
                    auto written = back.remaining();
                    write_impl(begin, back, written);
                    buffers.emplace_back();
                    begin += written;
                    len -= written;
                }
            }
        }

        size_t forward(composed_buffer<T> *to, size_t count) {
            unsigned long from_size = end() - begin();
            if (from_size >= count) {
                to->write(read(begin() += count));
                return count;
            } else {
                to->write(read(end()));
                return from_size;
            }
        }

        // bridge with low-level API

        buffer_ref front_buffer() {
            if (buffers.size() == 0) {
                buffers.emplace_front();
            }

            return buffers.front();
        }

        buffer_ref back_buffer() {
            if (buffers.size() == 0 || buffers.back().remaining() == 0) {
                buffers.emplace_back();
            }

            return buffers.back();
        }

        void adjust_front() {
            if (buffers.front().size() == 0) {
                buffers.pop_front();
            }
        }

        void adjust_back() {
            if (buffers.back().size() == 0) {
                buffers.pop_back();
            }
        }

    private:
        std::deque<util::simple_buffer<T>> buffers;

        void write_impl(std::vector<char>::const_iterator const &it, simple_buffer<char> &buf, size_t size) {
            for (size_t i = 0; i < size; i++) {
                *(buf.end() + i) = *(it + i);
            }
            buf.push(size);
            return;
        }
    };


}

#endif //PROXY_COMPOSED_BUFFER_H
