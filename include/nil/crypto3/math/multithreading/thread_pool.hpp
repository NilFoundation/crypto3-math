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

            enum class PoolID {
                LOW_LEVEL_POOL_ID,
                HIGH_LEVEL_POOL_ID
            };

            /** Returns a thread pool, based on the pool_id. pool with LOW_LEVEL_POOL_ID is normally used for low-level operations, like polynomial
             *  operations and fft. Any code that uses these operations and needs to be parallel will submit it's tasks to pool with HIGH_LEVEL_POOL_ID.
             *  Submission of higher level tasks to low level pool will immediately result to a deadlock.
             */
            static ThreadPool& get_instance(PoolID pool_id, std::size_t pool_size = std::thread::hardware_concurrency()) {
                static ThreadPool instance_for_low_level(PoolID::LOW_LEVEL_POOL_ID, pool_size);
                static ThreadPool instance_for_higher_level(PoolID::HIGH_LEVEL_POOL_ID, pool_size);
                
                if (pool_id == PoolID::LOW_LEVEL_POOL_ID)
                    return instance_for_low_level;
                if (pool_id == PoolID::HIGH_LEVEL_POOL_ID)
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
                std::size_t element_per_cpu = elements_count / cpu_usage;

                // Pool #0 will take care of the lowest level of operations, like polynomial operations.
                // We want the minimal size of element_per_cpu to be 65536, otherwise the cores are not loaded.
                if (pool_id == PoolID::LOW_LEVEL_POOL_ID && element_per_cpu < POOL_0_MIN_CHUNK_SIZE) {
                    cpu_usage = elements_count / POOL_0_MIN_CHUNK_SIZE + elements_count % POOL_0_MIN_CHUNK_SIZE ? 1 : 0;
                    element_per_cpu = elements_count / cpu_usage;
                }

                std::size_t begin = 0;
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
            inline ThreadPool(PoolID pool_id, std::size_t pool_size)
                : pool(pool_size)
                , pool_size(pool_size)
                , pool_id(pool_id) {
            }

            boost::asio::thread_pool pool;
            std::size_t pool_size;

            PoolID pool_id; 

            // For pool #0 we have experimentally found that operations over chunks of <65536 elements
            // do not load the cores. In case we have smaller chunks, it's better to load less cores.
            const std::size_t POOL_0_MIN_CHUNK_SIZE = 65536;
        };

    }        // namespace crypto3
}    // namespace nil

#endif // CRYPTO3_THREAD_POOL_HPP
