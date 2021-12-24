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

#ifndef CRYPTO3_MATH_EXPRESSION_AST_HPP
#define CRYPTO3_MATH_EXPRESSION_AST_HPP

#ifndef CRYPTO3_MATH_EXPRESSION_HPP
#error "ast.hpp must not be included directly!"
#endif

#include <list>
#include <string>

#include <boost/variant.hpp>

namespace nil {
    namespace crypto3 {
        namespace math {
            namespace expressions {
                namespace detail {
                    namespace ast {

                        struct nil {};
                        struct unary_op;
                        struct binary_op;
                        struct expression;

                        template <typename ValueType>
                        // clang-format off
                        typedef boost::variant<
                                nil // can't happen!
                                , ValueType
                                , std::string
                                , boost::recursive_wrapper<unary_op>
                                , boost::recursive_wrapper<binary_op>
                                , boost::recursive_wrapper<expression>
                                >
                        operand;
                        // clang-format on

                        template <typename ValueType>
                        struct unary_op {
                            ValueType (*op)(ValueType);
                            operand rhs;
                            unary_op() {}
                            unary_op(ValueType (*op)(ValueType), operand const &rhs) : op(op), rhs(rhs) {}
                        };

                        template <typename ValueType>
                        struct binary_op {
                            ValueType (*op)(ValueType, ValueType);
                            operand lhs;
                            operand rhs;
                            binary_op() {}
                            binary_op(ValueType (*op)(ValueType, ValueType), operand const &lhs,
                                      operand const &rhs)
                                : op(op), lhs(lhs), rhs(rhs) {}
                        };

                        template <typename ValueType>
                        struct operation {
                            ValueType (*op)(ValueType, ValueType);
                            operand rhs;
                            operation() {}
                            operation(ValueType (*op)(ValueType, ValueType), operand const &rhs)
                                : op(op), rhs(rhs) {}
                        };

                        template <typename ValueType>
                        struct expression {
                            operand lhs;
                            std::list<operation> rhs;
                            expression() {}
                            expression(operand const &lhs, std::list<operation> const &rhs)
                                : lhs(lhs), rhs(rhs) {}
                        };

                    } // namespace ast
                }    // namespace detail    
            }    // namespace expressions
        }    // namespace math
    }        // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_MATH_EXPRESSION_AST_HPP