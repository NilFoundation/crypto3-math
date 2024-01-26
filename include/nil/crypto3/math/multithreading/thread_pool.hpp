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
#include <boost/asio/post.hpp>

#include <functional>
#include <future>
#include <thread>
#include <limits>
#include <memory>
#include <stdexcept>

namespace nil {
    namespace crypto3 {

        template<class ReturnType>
        void wait_for_all(const std::vector<std::future<ReturnType>>& futures) {
            for (auto& f: futures) {
                f.wait();
            }
        }

        class ThreadPool {
        public:

            static ThreadPool& get_instance(std::size_t pool_number, std::size_t pool_size = std::thread::hardware_concurrency()) {
                static std::map<std::size_t, std::unique_ptr<ThreadPool>> instances;

                if (instances.find(pool_number) == instances.end() || !instances[pool_number]) {
                    instances[pool_number].reset(new ThreadPool(pool_number, pool_size));
                }
                return *instances[pool_number]; 
            }

            ThreadPool(const ThreadPool& obj)= delete;
            ThreadPool& operator=(const ThreadPool& obj)= delete;

            template<class ReturnType>
            inline std::future<ReturnType> post(std::function<ReturnType()> task) {
                auto packaged_task = std::make_shared<std::packaged_task<ReturnType()>>(std::move(task));
                std::future<ReturnType> fut = packaged_task->get_future();
                boost::asio::post(pool, [packaged_task]() -> void { (*packaged_task)(); });
                return fut;
            }
 
            // Waits for all the tasks to complete.
            inline void join() {
                pool.join();
            }

            // Divides work into a ranges and makes calls to func in parallel.
            template<class ReturnType>
            std::vector<std::future<ReturnType>> block_execution(
                    std::size_t elements_count,
                    std::function<ReturnType(std::size_t begin, std::size_t end)> func) {

                std::vector<std::future<ReturnType>> fut;
                std::size_t cpu_usage = std::min(elements_count, pool_size);
                std::size_t element_per_cpu = elements_count / cpu_usage;

                // Pool #0 will take care of the lowest level of operations, like polynomial operations.
                // We want the minimal size of element_per_cpu to be 65536, otherwise the cores are not loaded.
                if (pool_number == 0 && element_per_cpu < 65536) {
                    cpu_usage = elements_count / 65536 + elements_count % 65536 ? 1 : 0;
                    element_per_cpu = elements_count / cpu_usage;
                }

                for (int i = 0; i < cpu_usage; i++) {
                    auto begin = element_per_cpu * i;
                    auto end = (i == cpu_usage - 1) ? elements_count : element_per_cpu * (i + 1);
                    fut.emplace_back(post<ReturnType>([begin, end, func]() {
                        return func(begin, end);
                    }));
                }
                return fut;
            }

        private:
            inline ThreadPool(std::size_t pool_number, std::size_t pool_size)
                : pool(pool_size)
                , pool_size(pool_size)
                , pool_number(pool_number) {
            }

            boost::asio::thread_pool pool;
            std::size_t pool_size;

            // Each pool with know it's number.
            std::size_t pool_number;
        };

    }        // namespace crypto3
}    // namespace nil

#endif // CRYPTO3_THREAD_POOL_HPP
