//
// Created by radimir on 12/05/17.
//

#ifndef CPP_COURSE_BLOCKING_QUEUE_H
#define CPP_COURSE_BLOCKING_QUEUE_H


#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "interrupted.h"

namespace util
{

    template<typename T>
    class blocking_queue
    {
    private:
        std::queue<T> queue;
        std::mutex mutex;
        std::condition_variable cond;
        std::atomic<bool> interrupted{false};

    public:
        void push(T &&elem) {
            std::unique_lock<std::mutex> lock(mutex);

            queue.push(std::move(elem));
            if (queue.size() == 1)
                cond.notify_one();
        }

        void push(T const &elem)
        {
            std::unique_lock<std::mutex> lock(mutex);

            queue.push(elem);
            if (queue.size() == 1)
                cond.notify_one();
        }

        T pop()
        {
            std::unique_lock<std::mutex> lock(mutex);

            while (queue.empty() && !interrupted)
                cond.wait(lock);
            if (interrupted)
                throw util::interrupted{};
            T front = std::move(queue.front());
            queue.pop();
            return front;
        }

        void interrupt()
        {
            interrupted = true;
            cond.notify_all();
        }

        ~blocking_queue()
        {
            interrupt();
        }
    };
}

#endif //CPP_COURSE_BLOCKING_QUEUE_H
