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

#ifndef CRYPTO3_MATH_EXPRESSION_PARSER_DEF_HPP
#define CRYPTO3_MATH_EXPRESSION_PARSER_DEF_HPP

#include <cmath>
#include <iostream>
#include <limits>
#include <string>

#include <boost/math/constants/constants.hpp>
#include <boost/spirit/home/x3.hpp>

#include <nil/crypto3/math/expressions/ast.hpp>
#include <nil/crypto3/math/expressions/ast_adapted.hpp>
#include <nil/crypto3/math/expressions/math.hpp>
#include <nil/crypto3/math/expressions/parser.hpp>

namespace nil {
    namespace crypto3 {
        namespace math {
            namespace expressions {
                namespace detail {

                    namespace x3 = boost::spirit::x3;

                    namespace parser {

                        template <typename ValueType>
                        struct parser_def {

                            // LOOKUP

                            static struct constant_ : x3::symbols<ValueType> {
                                constant_() {
                                    // clang-format off
                                    add ("e"      , boost::math::constants::e<ValueType>())
                                        ("epsilon", std::numeric_limits<ValueType>::epsilon())
                                        ("phi"    , boost::math::constants::phi<ValueType>())
                                        ("pi"     , boost::math::constants::pi<ValueType>());
                                    // clang-format on
                                }
                            } constant;

                            static struct ufunc_ : x3::symbols<ValueType (*)(ValueType)> {
                                ufunc_() {
                                    // clang-format off
                                    add ("abs"   , static_cast<ValueType (*)(ValueType)>(&std::abs))
                                        ("acos"  , static_cast<ValueType (*)(ValueType)>(&std::acos))
                                        ("acosh" , static_cast<ValueType (*)(ValueType)>(&std::acosh))
                                        ("asin"  , static_cast<ValueType (*)(ValueType)>(&std::asin))
                                        ("asinh" , static_cast<ValueType (*)(ValueType)>(&std::asinh))
                                        ("atan"  , static_cast<ValueType (*)(ValueType)>(&std::atan))
                                        ("atanh" , static_cast<ValueType (*)(ValueType)>(&std::atanh))
                                        ("cbrt"  , static_cast<ValueType (*)(ValueType)>(&std::cbrt))
                                        ("ceil"  , static_cast<ValueType (*)(ValueType)>(&std::ceil))
                                        ("cos"   , static_cast<ValueType (*)(ValueType)>(&std::cos))
                                        ("cosh"  , static_cast<ValueType (*)(ValueType)>(&std::cosh))
                                        ("deg"   , static_cast<ValueType (*)(ValueType)>(&math::deg))
                                        ("erf"   , static_cast<ValueType (*)(ValueType)>(&std::erf))
                                        ("erfc"  , static_cast<ValueType (*)(ValueType)>(&std::erfc))
                                        ("exp"   , static_cast<ValueType (*)(ValueType)>(&std::exp))
                                        ("exp2"  , static_cast<ValueType (*)(ValueType)>(&std::exp2))
                                        ("floor" , static_cast<ValueType (*)(ValueType)>(&std::floor))
                                        ("isinf" , static_cast<ValueType (*)(ValueType)>(&math::isinf))
                                        ("isnan" , static_cast<ValueType (*)(ValueType)>(&math::isnan))
                                        ("log"   , static_cast<ValueType (*)(ValueType)>(&std::log))
                                        ("log2"  , static_cast<ValueType (*)(ValueType)>(&std::log2))
                                        ("log10" , static_cast<ValueType (*)(ValueType)>(&std::log10))
                                        ("rad"   , static_cast<ValueType (*)(ValueType)>(&math::rad))
                                        ("round" , static_cast<ValueType (*)(ValueType)>(&std::round))
                                        ("sgn"   , static_cast<ValueType (*)(ValueType)>(&math::sgn))
                                        ("sin"   , static_cast<ValueType (*)(ValueType)>(&std::sin))
                                        ("sinh"  , static_cast<ValueType (*)(ValueType)>(&std::sinh))
                                        ("sqrt"  , static_cast<ValueType (*)(ValueType)>(&std::sqrt))
                                        ("tan"   , static_cast<ValueType (*)(ValueType)>(&std::tan))
                                        ("tanh"  , static_cast<ValueType (*)(ValueType)>(&std::tanh))
                                        ("tgamma", static_cast<ValueType (*)(ValueType)>(&std::tgamma));
                                    // clang-format on
                                }
                            } ufunc;

                            static struct bfunc_ : x3::symbols<ValueType (*)(ValueType, ValueType)> {
                                bfunc_() {
                                    // clang-format off
                                    add ("atan2", static_cast<ValueType (*)(ValueType, ValueType)>(&std::atan2))
                                        ("max"  , static_cast<ValueType (*)(ValueType, ValueType)>(&std::fmax))
                                        ("min"  , static_cast<ValueType (*)(ValueType, ValueType)>(&std::fmin))
                                        ("pow"  , static_cast<ValueType (*)(ValueType, ValueType)>(&std::pow));
                                    // clang-format on
                                }
                            } bfunc;

                            static struct unary_op_ : x3::symbols<ValueType (*)(ValueType)> {
                                unary_op_() {
                                    // clang-format off
                                    add ("+", static_cast<ValueType (*)(ValueType)>(&math::plus))
                                        ("-", static_cast<ValueType (*)(ValueType)>(&math::minus))
                                        ("!", static_cast<ValueType (*)(ValueType)>(&math::unary_not));
                                    // clang-format on
                                }
                            } unary_op;

                            static struct additive_op_ : x3::symbols<ValueType (*)(ValueType, ValueType)> {
                                additive_op_() {
                                    // clang-format off
                                    add ("+", static_cast<ValueType (*)(ValueType, ValueType)>(&math::plus))
                                        ("-", static_cast<ValueType (*)(ValueType, ValueType)>(&math::minus));
                                    // clang-format on
                                }
                            } additive_op;

                            static struct multiplicative_op_ : x3::symbols<ValueType (*)(ValueType, ValueType)> {
                                multiplicative_op_() {
                                    // clang-format off
                                    add ("*", static_cast<ValueType (*)(ValueType, ValueType)>(&math::multiplies))
                                        ("/", static_cast<ValueType (*)(ValueType, ValueType)>(&math::divides))
                                        ("%", static_cast<ValueType (*)(ValueType, ValueType)>(&std::fmod));
                                    // clang-format on
                                }
                            } multiplicative_op;

                            static struct logical_op_ : x3::symbols<ValueType (*)(ValueType, ValueType)> {
                                logical_op_() {
                                    // clang-format off
                                    add ("&&", static_cast<ValueType (*)(ValueType, ValueType)>(&math::logical_and))
                                        ("||", static_cast<ValueType (*)(ValueType, ValueType)>(&math::logical_or));
                                    // clang-format on
                                }
                            } logical_op;

                            static struct relational_op_ : x3::symbols<ValueType (*)(ValueType, ValueType)> {
                                relational_op_() {
                                    // clang-format off
                                    add ("<" , static_cast<ValueType (*)(ValueType, ValueType)>(&math::less))
                                        ("<=", static_cast<ValueType (*)(ValueType, ValueType)>(&math::less_equals))
                                        (">" , static_cast<ValueType (*)(ValueType, ValueType)>(&math::greater))
                                        (">=", static_cast<ValueType (*)(ValueType, ValueType)>(&math::greater_equals));
                                    // clang-format on
                                }
                            } relational_op;

                            static struct equality_op_ : x3::symbols<ValueType (*)(ValueType, ValueType)> {
                                equality_op_() {
                                    // clang-format off
                                    add ("==", static_cast<ValueType (*)(ValueType, ValueType)>(&math::equals))
                                        ("!=", static_cast<ValueType (*)(ValueType, ValueType)>(&math::not_equals));
                                    // clang-format on
                                }
                            } equality_op;

                            static struct power_ : x3::symbols<ValueType (*)(ValueType, ValueType)> {
                                power_() {
                                    // clang-format off
                                    add ("**", static_cast<ValueType (*)(ValueType, ValueType)>(&std::pow));
                                    // clang-format on
                                }
                            } power;

                            // ADL markers

                            struct expression_class;
                            struct logical_class;
                            struct equality_class;
                            struct relational_class;
                            struct additive_class;
                            struct multiplicative_class;
                            struct factor_class;
                            struct primary_class;
                            struct unary_class;
                            struct binary_class;
                            struct variable_class;

                            // clang-format off

                            // Rule declarations

                            static auto const expression     = x3::rule<expression_class    , ast::expression>{"expression"};
                            static auto const logical        = x3::rule<logical_class       , ast::expression>{"logical"};
                            static auto const equality       = x3::rule<equality_class      , ast::expression>{"equality"};
                            static auto const relational     = x3::rule<relational_class    , ast::expression>{"relational"};
                            static auto const additive       = x3::rule<additive_class      , ast::expression>{"additive"};
                            static auto const multiplicative = x3::rule<multiplicative_class, ast::expression>{"multiplicative"};
                            static auto const factor         = x3::rule<factor_class        , ast::expression>{"factor"};
                            static auto const primary        = x3::rule<primary_class       , ast::operand   >{"primary"};
                            static auto const unary          = x3::rule<unary_class         , ast::unary_op  >{"unary"};
                            static auto const binary         = x3::rule<binary_class        , ast::binary_op >{"binary"};
                            static auto const variable       = x3::rule<variable_class      , std::string    >{"variable"};

                            // Rule defintions

                            static auto const expression_def =
                                logical;

                            static auto const logical_def =
                                equality >> *(logical_op > equality);

                            static auto const equality_def =
                                relational >> *(equality_op > relational);

                            static auto const relational_def =
                                additive >> *(relational_op > additive);

                            static auto const additive_def =
                                multiplicative >> *(additive_op > multiplicative);

                            static auto const multiplicative_def =
                                factor >> *(multiplicative_op > factor);

                            static auto const factor_def =
                                primary >> *( power > factor );

                            static auto const unary_def =
                                ufunc > '(' > expression > ')';

                            static auto const binary_def =
                                bfunc > '(' > expression > ',' > expression > ')';

                            constexpr static auto const variable_def =
                                x3::raw[x3::lexeme[x3::alpha >> *(x3::alnum | '_')]];

                            static auto const primary_def =
                                  x3::double_
                                | ('(' > expression > ')')
                                | (unary_op > primary)
                                | binary
                                | unary
                                | constant
                                | variable;

                            BOOST_SPIRIT_DEFINE(
                                expression,
                                logical,
                                equality,
                                relational,
                                additive,
                                multiplicative,
                                factor,
                                primary,
                                unary,
                                binary,
                                variable
                            )

                            // clang-format on

                            struct expression_class {
                                template <typename Iterator, typename Exception, typename Context>
                                x3::error_handler_result on_error(Iterator &, Iterator const &last,
                                                                  Exception const &x, Context const &) {
                                    std::cout << "Expected " << x.which() << " at \""
                                              << std::string{x.where(), last} << "\"" << std::endl;
                                    return x3::error_handler_result::fail;
                                }
                            };

                            using expression_type = x3::rule<expression_class, ast::expression>;

                            BOOST_SPIRIT_DECLARE(expression_type)

                        };

                    } // namespace parser

                    template <typename ValueType>
                    typename parser::parser_def<ValueType>::expression_type grammar() { 
                        return parser::parser_def<ValueType>::expression;
                    }

                }    // namespace detail    
            }    // namespace expressions
        }    // namespace math
    }        // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_MATH_EXPRESSION_PARSER_DEF_HPP