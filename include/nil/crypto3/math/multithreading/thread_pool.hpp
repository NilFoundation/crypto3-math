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

        class ThreadPool {
        public:

            enum class PoolLevel {
                LOW,
                HIGH
            };

            /** Returns a thread pool, based on the pool_id. pool with LOW is normally used for low-level operations, like polynomial
             *  operations and fft. Any code that uses these operations and needs to be parallel will submit its tasks to pool with HIGH.
             *  Submission of higher level tasks to low level pool will immediately result in a deadlock.
             */
            static ThreadPool& get_instance(PoolLevel pool_id, std::size_t pool_size = std::thread::hardware_concurrency()) {
                static ThreadPool instance_for_low_level(PoolLevel::LOW, pool_size);
                static ThreadPool instance_for_higher_level(PoolLevel::HIGH, pool_size);
                
                if (pool_id == PoolLevel::LOW)
                    return instance_for_low_level;
                if (pool_id == PoolLevel::HIGH)
                    return instance_for_higher_level;
                throw std::invalid_argument("Invalid instance of thread pool requested.");
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

            // Divides work into chunks and makes calls to 'func' in parallel.
            template<class ReturnType>
            std::vector<std::future<ReturnType>> block_execution(
                    std::size_t elements_count,
                    std::function<ReturnType(std::size_t begin, std::size_t end)> func) {

                std::vector<std::future<ReturnType>> fut;
                std::size_t cpu_usage = std::max((size_t)1, std::min(elements_count, pool_size));

                // Pool #0 will take care of the lowest level of operations, like polynomial operations.
                // We want the minimal size of elements_per_cpu to be 'POOL_0_MIN_CHUNK_SIZE', otherwise the cores are not loaded.
                if (pool_id == PoolLevel::LOW && elements_count / cpu_usage < POOL_0_MIN_CHUNK_SIZE) {
                    cpu_usage = elements_count / POOL_0_MIN_CHUNK_SIZE + elements_count % POOL_0_MIN_CHUNK_SIZE ? 1 : 0;
                }
                const std::size_t elements_per_cpu = elements_count / cpu_usage;

                std::size_t begin = 0;
                for (std::size_t i = 0; i < cpu_usage; i++) {
                    auto end = begin + (elements_count - begin) / (cpu_usage - i);
                    fut.emplace_back(post<ReturnType>([begin, end, func]() {
                        return func(begin, end);
                    }));
                    begin = end;
                }
                return fut;
            }

        private:
            inline ThreadPool(PoolLevel pool_id, std::size_t pool_size)
                : pool(pool_size)
                , pool_size(pool_size)
                , pool_id(pool_id) {
            }

            boost::asio::thread_pool pool;
            const std::size_t pool_size;

            const PoolLevel pool_id; 

            // For pool #0 we have experimentally found that operations over chunks of <65536 elements
            // do not load the cores. In case we have smaller chunks, it's better to load less cores.
            static constexpr std::size_t POOL_0_MIN_CHUNK_SIZE = 65536;
        };

    }        // namespace crypto3
}    // namespace nil

#endif // CRYPTO3_THREAD_POOL_HPP
