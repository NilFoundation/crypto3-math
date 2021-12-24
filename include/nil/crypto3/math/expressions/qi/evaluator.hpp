//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
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

#ifndef CRYPTO3_MATH_EXPRESSION_EVALUATOR_HPP
#define CRYPTO3_MATH_EXPRESSION_EVALUATOR_HPP

#ifndef CRYPTO3_MATH_EXPRESSION_HPP
#error "evaluator.hpp must not be included directly!"
#endif

#include <map>
#include <string>

#include <boost/fusion/include/adapt_struct.hpp>
#include <nil/crypto3/math/expressions/ast.hpp>

namespace nil {
    namespace crypto3 {
        namespace math {
            namespace expressions {
                namespace detail {
                    namespace ast {

                        // Optimizer

                        template <typename T, typename U>
                        struct is_same {
                            static const bool value = false;
                        };

                        template <typename T>
                        struct is_same<T, T> {
                            static const bool value = true;
                        };

                        template <typename T>
                        struct holds_alternative_impl {
                            typedef bool result_type;

                            template <typename U>
                            bool operator()(U const & /*unused*/) const {
                                return is_same<U, T>::value;
                            }
                        };

                        template <typename ValueType>
                        bool holds_alternative(operand<ValueType> const &v) {
                            return boost::apply_visitor(holds_alternative_impl<ValueType>(), v);
                        }

                        template <typename ValueType>
                        struct ConstantFolder {
                            typedef operand result_type;

                            result_type operator()(nil) const { return 0; }

                            result_type operator()(ValueType n) const {
                                return n;
                            }

                            result_type operator()(std::string const &c) const{
                                return c;
                            }

                            result_type operator()(operation<ValueType> const &x, operand<ValueType> const &lhs) const {
                                operand<ValueType> rhs = boost::apply_visitor(*this, x.rhs);

                                if (holds_alternative<ValueType>(lhs) && holds_alternative<ValueType>(rhs)) {
                                    return x.op(boost::get<ValueType>(lhs), boost::get<ValueType>(rhs));
                                }
                                return binary_op<ValueType>(x.op, lhs, rhs);
                            }

                            result_type operator()(unary_op<ValueType> const &x) const {
                                operand<ValueType> rhs = boost::apply_visitor(*this, x.rhs);

                                /// If the operand is known, we can directly evaluate the function.
                                if (holds_alternative<ValueType>(rhs)) {
                                    return x.op(boost::get<ValueType>(rhs));
                                }
                                return unary_op<ValueType>(x.op, rhs);
                            }

                            result_type operator()(binary_op<ValueType> const &x) const {
                                operand<ValueType> lhs = boost::apply_visitor(*this, x.lhs);
                                operand<ValueType> rhs = boost::apply_visitor(*this, x.rhs);

                                /// If both operands are known, we can directly evaluate the function,
                                /// else we just update the children with the new expressions.
                                if (holds_alternative<ValueType>(lhs) && holds_alternative<ValueType>(rhs)) {
                                    return x.op(boost::get<ValueType>(lhs), boost::get<ValueType>(rhs));
                                }
                                return binary_op<ValueType>(x.op, lhs, rhs);
                            }

                            result_type operator()(expression<ValueType> const &x) const {
                                operand<ValueType> state = boost::apply_visitor(*this, x.lhs);
                                for (std::list<operation<ValueType>>::const_iterator it = x.rhs.begin();
                                     it != x.rhs.end(); ++it) {
                                    state = (*this)(*it, state);
                                }
                                return state;
                            }
                        };

                        template <typename ValueType>
                        struct eval {
                            typedef ValueType result_type;

                            explicit eval(std::map<std::string, ValueType> const &sym) : st(sym) {}

                            ValueType operator()(nil) const {
                                BOOST_ASSERT(0);
                                return 0;
                            }

                            ValueType operator()(ValueType n) const { return n; }

                            ValueType operator()(std::string const &c) const {
                                std::map<std::string, ValueType>::const_iterator it = st.find(c);
                                if (it == st.end()) {
                                    throw std::invalid_argument("Unknown variable " + c); // NOLINT
                                }
                                return it->second;
                            }

                            ValueType operator()(operation<ValueType> const &x, ValueType lhs) const {
                                ValueType rhs = boost::apply_visitor(*this, x.rhs);
                                return x.op(lhs, rhs);
                            }

                            ValueType operator()(unary_op<ValueType> const &x) const {
                                ValueType rhs = boost::apply_visitor(*this, x.rhs);
                                return x.op(rhs);
                            }

                            ValueType operator()(binary_op<ValueType> const &x) const {
                                ValueType lhs = boost::apply_visitor(*this, x.lhs);
                                ValueType rhs = boost::apply_visitor(*this, x.rhs);
                                return x.op(lhs, rhs);
                            }

                            ValueType operator()(expression<ValueType> const &x) const {
                                ValueType state = boost::apply_visitor(*this, x.lhs);
                                for (std::list<operation<ValueType>>::const_iterator it = x.rhs.begin();
                                     it != x.rhs.end(); ++it) {
                                    state = (*this)(*it, state);
                                }
                                return state;
                            }

                        private:
                            std::map<std::string, ValueType> st;
                        };

                    } // namespace ast
                }    // namespace detail    
            }    // namespace expressions
        }    // namespace math
    }        // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_MATH_EXPRESSION_EVALUATOR_HPP