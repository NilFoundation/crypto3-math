//---------------------------------------------------------------------------//
// Copyright (c) 2023 Martun Karapetyan <martun@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_THREAD_POOL_HPP
#define CRYPTO3_THREAD_POOL_HPP

#include <boost/cobalt.hpp>

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/thread_pool.hpp>


namespace nil {
    namespace crypto3 {
        class ThreadPool {
        public:

            static std::unique_ptr<ThreadPool> instance;

            static void start(std::size_t pool_size) {
                instance.reset(new ThreadPool(pool_size));
            }

            static ThreadPool& get_instance() {
                if (!instance) {
                    throw std::logic_error("Getting instance of a thread pool before it was started.");
                }
                return *instance; 
            }
 
co_await cobalt::join(tasks);

            // Divides work into a ranges and makes calls to func in parallel.
            template<class ReturnType>
            std::vector<cobalt::task<ReturnType>> block_execution(
                    std::size_t elements_count,
                    std::function<ReturnType(std::size_t begin, std::size_t end)> func) {

                auto exec = co_await cobalt::this_coro::executor;

                std::vector<cobalt::task<ReturnType>> tasks;
                std::size_t cpu_usage = std::min(elements_count, thread_count);
                std::size_t element_per_cpu = elements_count / cpu_usage;

                for (int i = 0; i < cpu_usage; i++) {
                    auto begin = element_per_cpu * i;
                    auto end = (i == cpu_usage - 1) ? elements_count : element_per_cpu * (i + 1);
                    tasks.emplace_back(spawn(ctx, [begin, end, func]() {
                        co_return func(begin, end);
                    }, asio::use_future));
                }
                return tasks;
            }

        private: 
            ThreadPool(std::size_t thread_count)
                : thread_count(thread_count)
                , tp(thread_count)
            {
                ctx.run();
            }

            asio::io_context ctx{BOOST_ASIO_CONCURRENCY_HINT_1};
            const std::size_t thread_count;
            boost::asio::thread_pool tp;

        };
    }        // namespace crypto3
}    // namespace nil

#endif // CRYPTO3_THREAD_POOL_HPP
