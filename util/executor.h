//
// Created by radimir on 12/05/17.
//

#ifndef CPP_COURSE_EXECUTOR_H
#define CPP_COURSE_EXECUTOR_H

#include <functional>
#include <thread>
#include "blocking_queue.h"
#include "log.h"

namespace util
{
    class executor
    {
    private:
        using Task = std::function<void()>;
        log executor_log;
        blocking_queue<Task> queue;
        std::vector<std::thread> threads;

    public:
        executor(size_t threads_n)
                : executor_log{"executor"}, queue{}, threads{}
        {
            threads.reserve(threads_n);
            try {
                for (size_t i = 0; i < threads_n; ++i) {
                    threads.emplace_back([&]() {
                        Task task;
                        while (true) {
                            try {
                                task = queue.pop();
                            } catch (interrupted const &) {
                                return;
                            }
                            try {
                                task();
                            } catch (std::exception const &e) {
                                executor_log(ERROR) << "Got exception on executing task '" << e.what() << ".\n";
                            }
                        }
                    });
                }
            } catch (std::exception const &e) {
                executor_log(ERROR) << "Got exception on startup '" << e.what() << "'. Halting.";
            }
        }

        void consume(Task t) noexcept
        {
            queue.push(std::move(t));
        }

        void join()
        {
            queue.interrupt();
            for (auto &&thread : threads) {
                thread.join();
            }
        }

        ~executor()
        {
            join();
        }
    };
}

#endif //CPP_COURSE_EXECUTOR_H
