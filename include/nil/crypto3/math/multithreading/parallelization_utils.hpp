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

#ifndef CRYPTO3_PARALLELIZATION_UTILS_HPP
#define CRYPTO3_PARALLELIZATION_UTILS_HPP

#include <future>

#include <nil/crypto3/math/multithreading/thread_pool.hpp>

namespace nil {
    namespace crypto3 {

        template<class ReturnType>
        void wait_for_all(const std::vector<std::future<ReturnType>>& futures) {
            for (auto& f: futures) {
                f.wait();
            }
        }

        // Similar to std::transform, but in parallel. We return void here for better usability for our use cases.
        template<class InputIt1, class InputIt2, class OutputIt, class BinaryOperation>
        void parallel_transform(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                OutputIt d_first, BinaryOperation binary_op,
                                ThreadPool::PoolLevel pool_id = ThreadPool::PoolLevel::LOW) {

            wait_for_all(ThreadPool::get_instance(pool_id).block_execution<void>(
                std::distance(first1, last1),
                // We need the lambda to be mutable, to be able to modify iterators captured by value.
                [first1, last1, first2, d_first, binary_op](std::size_t begin, std::size_t end) mutable {
                    std::advance(first1, begin);
                    std::advance(first2, begin);
                    std::advance(d_first, begin);
                    for (std::size_t i = begin; i < end && first1 != last1; i++) {
                        *d_first = binary_op(*first1, *first2);
                        ++first1;
                        ++first2;
                        ++d_first;
                    }
                }));
        }

        // Similar to std::transform, but in parallel. We return void here for better usability for our use cases.
        template< class InputIt, class OutputIt, class UnaryOperation >
        void parallel_transform(InputIt first1, InputIt last1,
                                OutputIt d_first, UnaryOperation unary_op,
                                ThreadPool::PoolLevel pool_id = ThreadPool::PoolLevel::LOW) {

            wait_for_all(ThreadPool::get_instance(pool_id).block_execution<void>(
                std::distance(first1, last1),
                // We need the lambda to be mutable, to be able to modify iterators captured by value.
                [first1, last1, d_first, unary_op](std::size_t begin, std::size_t end) mutable {
                    std::advance(first1, begin);
                    std::advance(d_first, begin);
                    for (std::size_t i = begin; i < end && first1 != last1; i++) {
                        *d_first = unary_op(*first1);
                        ++first1;
                        ++d_first;
                    }
                }));
        }

        // This one is an optimization, since copying field elements is quite slow.
        // BinaryOperation is supposed to modify the object in-place.
        template<class InputIt1, class InputIt2, class BinaryOperation>
        void in_place_parallel_transform(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                         BinaryOperation binary_op,
                                         ThreadPool::PoolLevel pool_id = ThreadPool::PoolLevel::LOW) {

            wait_for_all(ThreadPool::get_instance(pool_id).block_execution<void>(
                std::distance(first1, last1),
                // We need the lambda to be mutable, to be able to modify iterators captured by value.
                [first1, last1, first2, binary_op](std::size_t begin, std::size_t end) mutable {
                    std::advance(first1, begin);
                    std::advance(first2, begin);
                    for (std::size_t i = begin; i < end && first1 != last1; i++) {
                        binary_op(*first1, *first2);
                        ++first1;
                        ++first2;
                    }
                }));
        }

        // This one is an optimization, since copying field elements is quite slow.
        // UnaryOperation is supposed to modify the object in-place.
        template<class InputIt, class UnaryOperation>
        void parallel_foreach(InputIt first1, InputIt last1, UnaryOperation unary_op,
                              ThreadPool::PoolLevel pool_id = ThreadPool::PoolLevel::LOW) {

            wait_for_all(ThreadPool::get_instance(pool_id).block_execution<void>(
                std::distance(first1, last1),
                // We need the lambda to be mutable, to be able to modify iterators captured by value.
                [first1, last1, unary_op](std::size_t begin, std::size_t end) mutable {
                    std::advance(first1, begin);
                    for (std::size_t i = begin; i < end && first1 != last1; i++) {
                        unary_op(*first1);
                        ++first1;
                    }
                }));
        }

        // Calls function func for each value between [start, end).
        void parallel_for(std::size_t start, std::size_t end, std::function<void(std::size_t index)> func,
                          ThreadPool::PoolLevel pool_id = ThreadPool::PoolLevel::LOW) {

            wait_for_all(ThreadPool::get_instance(pool_id).block_execution<void>(
                end - start,
                [start, func](std::size_t range_begin, std::size_t range_end) {
                    for (std::size_t i = start + range_begin; i < start + range_end; i++) {
                        func(i);
                    }
                }));
        }

    }        // namespace crypto3
}    // namespace nil

#endif // CRYPTO3_PARALLELIZATION_UTILS_HPP
