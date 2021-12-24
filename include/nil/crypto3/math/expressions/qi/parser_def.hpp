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

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#include <boost/spirit/include/qi.hpp>

namespace nil {
    namespace crypto3 {
        namespace math {
            namespace expressions {
                namespace detail {

                    namespace qi = boost::spirit::qi;

                    namespace parser {

                        struct expectation_handler {
                        template <typename>
                        struct result {
                            typedef void type;
                        };

                        template <typename Iterator>
                        void operator()(Iterator first, Iterator last,
                                        boost::spirit::info const &info) const {
                            std::stringstream msg;
                            msg << "Expected " << info << " at \"" << std::string(first, last)
                                << "\"";

                            throw std::runtime_error(msg.str()); // NOLINT
                        }
                    };

                    template <typename Iterator>
                    struct grammar : qi::grammar<Iterator, ast::expression(), ascii::space_type> {
                        expectation_handler err_handler;
                        qi::rule<Iterator, ast::expression(), ascii::space_type> expression,
                            logical, equality, relational, additive, multiplicative, factor;
                        qi::rule<Iterator, ast::operand(), ascii::space_type> primary;
                        qi::rule<Iterator, ast::unary_op(), ascii::space_type> unary;
                        qi::rule<Iterator, ast::binary_op(), ascii::space_type> binary;
                        qi::rule<Iterator, std::string()> variable;

                        qi::symbols<typename std::iterator_traits<Iterator>::value_type, double>
                            constant;
                        qi::symbols<typename std::iterator_traits<Iterator>::value_type,
                                    double (*)(double)>
                            ufunc, unary_op;
                        qi::symbols<typename std::iterator_traits<Iterator>::value_type,
                                    double (*)(double, double)>
                            bfunc, additive_op, multiplicative_op, logical_op, relational_op, equality_op, power;

                        grammar();
                    };

                        template <typename Iterator>
                        grammar<Iterator>::grammar() : grammar::base_type(expression) {
                            qi::_2_type _2;
                            qi::_3_type _3;
                            qi::_4_type _4;

                            qi::alnum_type alnum;
                            qi::alpha_type alpha;
                            qi::double_type double_;
                            qi::lexeme_type lexeme;
                            qi::raw_type raw;

                            // clang-format off

                            constant.add
                                ("e"      , boost::math::constants::e<double>())
                                ("epsilon", std::numeric_limits<double>::epsilon())
                                ("phi"    , boost::math::constants::phi<double>())
                                ("pi"     , boost::math::constants::pi<double>())
                                ;

                            ufunc.add
                                ("abs"  , static_cast<double (*)(double)>(&std::abs))
                                ("acos" , static_cast<double (*)(double)>(&std::acos))
                                ("asin" , static_cast<double (*)(double)>(&std::asin))
                                ("atan" , static_cast<double (*)(double)>(&std::atan))
                                ("ceil" , static_cast<double (*)(double)>(&std::ceil))
                                ("cos"  , static_cast<double (*)(double)>(&std::cos))
                                ("cosh" , static_cast<double (*)(double)>(&std::cosh))
                                ("deg"  , static_cast<double (*)(double)>(&math::deg))
                                ("exp"  , static_cast<double (*)(double)>(&std::exp))
                                ("floor", static_cast<double (*)(double)>(&std::floor))
                                ("isinf", static_cast<double (*)(double)>(&math::isinf))
                                ("isnan", static_cast<double (*)(double)>(&math::isnan))
                                ("log"  , static_cast<double (*)(double)>(&std::log))
                                ("log10", static_cast<double (*)(double)>(&std::log10))
                                ("rad"  , static_cast<double (*)(double)>(&math::rad))
                                ("sgn"  , static_cast<double (*)(double)>(&math::sgn))
                                ("sin"  , static_cast<double (*)(double)>(&std::sin))
                                ("sinh" , static_cast<double (*)(double)>(&std::sinh))
                                ("sqrt" , static_cast<double (*)(double)>(&std::sqrt))
                                ("tan"  , static_cast<double (*)(double)>(&std::tan))
                                ("tanh" , static_cast<double (*)(double)>(&std::tanh))
                                ;

                            bfunc.add
                                ("atan2", static_cast<double (*)(double, double)>(&std::atan2))
                                ("pow"  , static_cast<double (*)(double, double)>(&std::pow))
                                ;

                            unary_op.add
                                ("+", static_cast<double (*)(double)>(&math::plus))
                                ("-", static_cast<double (*)(double)>(&math::minus))
                                ("!", static_cast<double (*)(double)>(&math::unary_not))
                                ;

                            additive_op.add
                                ("+", static_cast<double (*)(double, double)>(&math::plus))
                                ("-", static_cast<double (*)(double, double)>(&math::minus))
                                ;

                            multiplicative_op.add
                                ("*", static_cast<double (*)(double, double)>(&math::multiplies))
                                ("/", static_cast<double (*)(double, double)>(&math::divides))
                                ("%", static_cast<double (*)(double, double)>(&std::fmod))
                                ;

                            logical_op.add
                                ("&&", static_cast<double (*)(double, double)>(&math::logical_and))
                                ("||", static_cast<double (*)(double, double)>(&math::logical_or))
                                ;

                            relational_op.add
                                ("<" , static_cast<double (*)(double, double)>(&math::less))
                                ("<=", static_cast<double (*)(double, double)>(&math::less_equals))
                                (">" , static_cast<double (*)(double, double)>(&math::greater))
                                (">=", static_cast<double (*)(double, double)>(&math::greater_equals))
                                ;

                            equality_op.add
                                ("==", static_cast<double (*)(double, double)>(&math::equals))
                                ("!=", static_cast<double (*)(double, double)>(&math::not_equals))
                                ;

                            power.add
                                ("**", static_cast<double (*)(double, double)>(&std::pow))
                                ;

                            expression =
                                logical.alias()
                                ;

                            logical =
                                equality >> *(logical_op > equality)
                                ;

                            equality =
                                relational >> *(equality_op > relational)
                                ;

                            relational =
                                additive >> *(relational_op > additive)
                                ;

                            additive =
                                multiplicative >> *(additive_op > multiplicative)
                                ;

                            multiplicative =
                                factor >> *(multiplicative_op > factor)
                                ;

                            factor =
                                primary >> *( power > factor )
                                ;

                            unary =
                                ufunc > '(' > expression > ')'
                                ;

                            binary =
                                bfunc > '(' > expression > ',' > expression > ')'
                                ;

                            variable =
                                raw[lexeme[alpha >> *(alnum | '_')]]
                                ;

                            primary =
                                  double_
                                | ('(' > expression > ')')
                                | (unary_op > primary)
                                | binary
                                | unary
                                | constant
                                | variable
                                ;

                            // clang-format on

                            expression.name("expression");
                            logical.name("logical");
                            equality.name("equality");
                            relational.name("relational");
                            additive.name("additive");
                            multiplicative.name("multiplicative");
                            factor.name("factor");
                            variable.name("variable");
                            primary.name("primary");
                            unary.name("unary");
                            binary.name("binary");

                            // typedef boost::phoenix::function<error_handler<Iterator> >
                            // error_handler_function; qi::on_error<qi::fail>(expression,
                            //        error_handler_function(error_handler<Iterator>())(
                            //            "Error! Expecting ", qi::_4, qi::_3));
                            qi::on_error<qi::fail>(
                                expression,
                                boost::phoenix::bind(boost::phoenix::ref(err_handler), _3, _2, _4));
                        }

                    } // namespace parser

                }    // namespace detail    
            }    // namespace expressions
        }    // namespace math
    }        // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_MATH_EXPRESSION_PARSER_DEF_HPP