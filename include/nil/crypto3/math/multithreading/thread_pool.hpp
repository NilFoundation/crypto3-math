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

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/use_future.hpp>

#include <functional>
#include <memory>
#include <future>
#include <stdexcept>

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

            ThreadPool(const ThreadPool& obj)= delete;
            ThreadPool& operator=(const ThreadPool& obj)= delete;

            // Waits for all the tasks to complete.
            inline void join() {
                pool.join();
            }

            // Divides work into a ranges and makes calls to func in parallel.
            boost::asio::awaitable<void> block_execution(
                    std::size_t elements_count,
                    std::function<void(std::size_t begin, std::size_t end)> func) {

                std::size_t cpu_usage = std::min(elements_count, pool_size);
                std::size_t element_per_cpu = elements_count / cpu_usage;

                std::vector<boost::asio::awaitable<void>> awaitables;
                for (int i = 0; i < cpu_usage; i++) {
                    auto begin = element_per_cpu * i;
                    auto end = (i == cpu_usage - 1) ? elements_count : element_per_cpu * (i + 1);

                    awaitables.emplace_back(std::move(boost::asio::co_spawn(pool,
                        [begin, end, func]()->boost::asio::awaitable<void> {
                            func(begin, end);
                            co_return;
                        },
                        boost::asio::use_awaitable)));
                }

                for (auto& awaitable: awaitables) {
                    co_await std::move(awaitable);
                }
            }

            template<class ReturnType>
            std::future<ReturnType> execute(boost::asio::awaitable<ReturnType>&& awaitable) {
                return boost::asio::co_spawn(pool, std::move(awaitable), boost::asio::use_future); 
            }

        private:
            inline ThreadPool(std::size_t pool_size)
                : pool(pool_size)
                , pool_size(pool_size) {
            }

            boost::asio::thread_pool pool;
            std::size_t pool_size;
        };

        std::unique_ptr<ThreadPool> ThreadPool::instance = nullptr;

    }        // namespace crypto3
}    // namespace nil

#endif // CRYPTO3_THREAD_POOL_HPP
