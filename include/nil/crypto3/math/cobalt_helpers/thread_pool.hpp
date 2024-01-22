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

#include <thread>

#include <boost/cobalt/promise.hpp>
#include <boost/cobalt/task.hpp>
#include <boost/cobalt/this_thread.hpp>
#include <boost/cobalt/join.hpp>

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

            static void start(std::size_t pool_size = std::thread::hardware_concurrency()) {
                instance.reset(new ThreadPool(pool_size));
            }

            static ThreadPool& get_instance() {
                if (!instance) {
                    start();
                }
                return *instance; 
            }
 
            template<class ReturnType>
            boost::cobalt::task<void> run_on_executor(
                std::size_t begin, std::size_t end,
                std::function<ReturnType(std::size_t begin, std::size_t end)> func,
                boost::asio::executor_arg_t = {}, boost::cobalt::executor = boost::cobalt::this_thread::get_executor()) {
                co_return func(begin, end);
            }           

            // Divides work into a ranges and makes calls to func in parallel.
            // We assume that func is free to submit tasks to the same thread pool.
            template<class ReturnType>
            boost::cobalt::task<std::vector<ReturnType>> block_execution(
                    std::size_t elements_count,
                    std::function<ReturnType(std::size_t begin, std::size_t end)> func) {

                auto exec = co_await boost::cobalt::this_coro::executor;

                std::vector<ReturnType> results;
                std::size_t cpu_usage = std::min(elements_count, thread_count);
                std::size_t element_per_cpu = elements_count / cpu_usage;

                std::vector<boost::cobalt::task<void>> tasks;

                for (int i = 0; i < cpu_usage; i++) {
                    auto begin = element_per_cpu * i;
                    auto end = (i == cpu_usage - 1) ? elements_count : element_per_cpu * (i + 1);

                    boost::cobalt::task<ReturnType> task = run_on_executor<ReturnType>(
                        begin, end, func, boost::asio::executor_arg, tp.get_executor());
                    tasks.emplace_back(std::move(task));
                }
                co_return boost::cobalt::join(tasks);
            }

            boost::cobalt::task<void> run_on_executor(
                std::size_t begin, std::size_t end,
                std::function<void(std::size_t begin, std::size_t end)> func,
                boost::asio::executor_arg_t = {}, boost::cobalt::executor = boost::cobalt::this_thread::get_executor()) {

                func(begin, end);
                co_return;
            }

            boost::cobalt::promise<void> block_execution(
                    std::size_t elements_count,
                    std::function<void(std::size_t begin, std::size_t end)> func) {

                auto exec = co_await boost::cobalt::this_coro::executor;

                std::size_t cpu_usage = std::min(elements_count, thread_count);
                std::size_t element_per_cpu = elements_count / cpu_usage;

                std::vector<boost::cobalt::task<void>> tasks;
                for (int i = 0; i < cpu_usage; i++) {
                    auto begin = element_per_cpu * i;
                    auto end = (i == cpu_usage - 1) ? elements_count : element_per_cpu * (i + 1);

                    boost::cobalt::task<void> task = run_on_executor(
                        begin, end, func, boost::asio::executor_arg, tp.get_executor());
                    tasks.emplace_back(std::move(task));
                }
                co_await boost::cobalt::join(tasks);
            }

	        ~ThreadPool() {
		        tp.join(); 
	        }

        private: 
            ThreadPool(std::size_t thread_count)
                : thread_count(thread_count)
                , tp(thread_count)
            {
            }

            const std::size_t thread_count;
            boost::asio::thread_pool tp;

        };
    }        // namespace crypto3
}    // namespace nil

#endif // CRYPTO3_THREAD_POOL_HPP
