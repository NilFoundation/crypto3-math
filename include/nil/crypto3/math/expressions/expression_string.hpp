//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2022 Ekaterina Chukavina <kate@nil.foundation>
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

#ifndef CRYPTO3_MATH_EXPRESSION_STRING_HPP
#define CRYPTO3_MATH_EXPRESSION_STRING_HPP
#include <iterator>
//
//#include <nil/crypto3/math/expressions/ast.hpp>
//#include <nil/crypto3/math/expressions/evaluator.hpp>
//#include <nil/crypto3/math/expressions/parser.hpp>
//#include <nil/crypto3/math/polynomial/polynomial.hpp>
//#include <nil/crypto3/math/expressions/x3/matheval.hpp>

/// @brief Parse a mathematical expression
///
/// This can parse and evaluate a mathematical expression for a given
/// symbol table using Boost.Spirit X3.  The templates of Boost.Spirit
/// are very expensive to parse and instantiate, which is why we hide
/// it behind an opaque pointer.
///
/// The drawback of this approach is that calls can no longer be
/// inlined and because the pointer crosses translation unit
/// boundaries, dereferencing it can also not be optimized out at
/// compile time.  We have to rely entirely on link-time optimization
/// which might be not as good.
///
/// The pointer to the implementation is a std::unique_ptr which makes
/// the class not copyable but only moveable.  Copying shouldn't be
/// required but is easy to implement.

#include <nil/crypto3/multiprecision/number.hpp>
#include <boost/regex.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <boost/metaparse/string.hpp>
using std::ostream;

namespace nil {
    namespace crypto3 {
        namespace math {
            namespace expressions_tstr {


                template<char... chars>
                using tstring = std::integer_sequence<char, chars...>;
                template<typename T, T... chars>
                constexpr tstring<chars...> operator""_tstr() {
                    return {};
                }

                template<typename T, T... Chars1, T... Chars2>
                constexpr tstring< Chars1...,' ', '+', ' ',Chars2...>operator+(tstring<Chars1...>,tstring<Chars2...>){
                    return std::integer_sequence<char, Chars1...,' ', '+', ' ',Chars2...>();
                }
                template<typename>
                class X;

                template<char... elements>
                class X<tstring< elements...>> {
                public:
                    // X(){}/
                    using bstr = boost::metaparse::string<elements...>;
                    static constexpr boost::metaparse::string<elements...> get_chars() {
                        constexpr const boost::metaparse::string<elements...>
                            s;    // char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        return s;
                    }

                    static constexpr const char* get_string() {
                        constexpr const char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        return str;
                    }

                    template <typename ValueType>
                    static constexpr ValueType parse_value_type(std::size_t start, std::size_t end) {
                        constexpr const char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        const char *space = " ";
                        //                        repeated<one_char>
                        std::size_t cnt = -1;
                        ValueType res = 0;
                        for (std::size_t i = start; i< end; i++){
                            if (str[i] != space[0]){
                                cnt+= 1;
                            }
                        }
                        for (std::size_t i = start; i< end; i++){
                            if (str[i] != space[0]){
                                res+=ValueType((int)(str[i])-(int)('0'))*ValueType(10).pow(cnt);//(int)(pow(10,cnt)) ;
                                                                                                          //    std::cout<<ValueType((int)(str[i])-(int)('0'))*ValueType(10).pow(cnt)<<std::endl;//((int)(str[i])-(int)('0'))*(int)(pow(10,cnt))<<std::endl;
                                cnt-=1;
                            }
                        }
                        return res;
                    }
                    static constexpr const char* str = get_string();

                    constexpr static const std::size_t get_str_len() {
                        constexpr char str[sizeof...(elements) + 1] = {elements..., '\0'};

                        std::size_t size = 0;
                        for (; str[size] != '\0'; size++) {
                        }
                        return size;
                    }

                    constexpr static const std::array<std::size_t, 4> count_ops() {
                        constexpr char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        std::size_t plus = 0;
                        std::size_t minus = 0;
                        std::size_t div = 0;
                        std::size_t mul = 0;
                        constexpr const char* div_str = "/";
                        constexpr const char* mul_str = "*";
                        constexpr const char* plus_str = "+";
                        constexpr const char* minus_str = "-";
                        size_t i = 0;
                        size_t cnt = 0;
                        using boost::spirit::qi::ascii::space;

                        for (; str[i] != '\0'; i++) {
                            if (str[i] == plus_str[0]) {
                                plus++;
                            } else {
                                if (str[i] == minus_str[0]) {
                                    minus++;
                                } else {
                                    if (str[i] == mul_str[0]) {
                                        mul++;
                                    } else {
                                        if (str[i] == div_str[0]) {
                                            div++;
                                        }
                                    }
                                }
                            }
                        }

                        std::array<std::size_t, 4> ops = {plus, minus, mul, div};
                        return ops;
                        // return cnt + 1;
                    }
                    //
                    static constexpr std::size_t find_str(const char* substr, std::size_t n, std::size_t start_pos,
                                                          std::size_t end_pos) {
                        constexpr char str[sizeof...(elements) + 1] = {elements..., '\0'};

                        std::size_t size = 0;
                        for (; str[size] != '\0'; size++) {
                        }
                        size_t j = 0;
                        size_t i = start_pos;
                        if (i + n < size) {
                            for (; i < end_pos; i++) {
                                for (j = 0; j < n && str[i + j] == substr[j]; j++)
                                    ;
                                if (j == n) {
                                    return i;
                                }
                            }
                        }
                        return std::string::npos;
                    }
                    static constexpr const std::array<std::size_t, 4> ops_cnt = count_ops();

                    static constexpr const std::size_t str_len = get_str_len();

                };
                template<typename>
                class X;

                template<char... elements>
                class X<const std::integer_sequence<char, elements...>> {
                public:
                    // X(){}/
                    using bstr = boost::metaparse::string<elements...>;
                    static constexpr boost::metaparse::string<elements...> get_chars() {
                        constexpr const boost::metaparse::string<elements...>
                            s;    // char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        return s;
                    }

                    static constexpr const char* get_string() {
                        constexpr const char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        return str;
                    }

                    template <typename ValueType>
                    static constexpr ValueType parse_value_type(std::size_t start, std::size_t end) {
                        constexpr const char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        const char *space = " ";
                        //                        repeated<one_char>
                        std::size_t cnt = -1;
                        ValueType res = 0;
                        for (std::size_t i = start; i< end; i++){
                            if (str[i] != space[0]){
                                cnt+= 1;
                            }
                        }
                        for (std::size_t i = start; i< end; i++){
                            if (str[i] != space[0]){
                                res+=ValueType((int)(str[i])-(int)('0'))*ValueType(10).pow(cnt);//(int)(pow(10,cnt)) ;
                                                                                                          //    std::cout<<ValueType((int)(str[i])-(int)('0'))*ValueType(10).pow(cnt)<<std::endl;//((int)(str[i])-(int)('0'))*(int)(pow(10,cnt))<<std::endl;
                                cnt-=1;
                            }
                        }
                        return res;
                    }
                    static constexpr const char* str = get_string();

                    constexpr static const std::size_t get_str_len() {
                        constexpr char str[sizeof...(elements) + 1] = {elements..., '\0'};

                        std::size_t size = 0;
                        for (; str[size] != '\0'; size++) {
                        }
                        return size;
                    }

                    constexpr static const std::array<std::size_t, 4> count_ops() {
                        constexpr char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        std::size_t plus = 0;
                        std::size_t minus = 0;
                        std::size_t div = 0;
                        std::size_t mul = 0;
                        constexpr const char* div_str = "/";
                        constexpr const char* mul_str = "*";
                        constexpr const char* plus_str = "+";
                        constexpr const char* minus_str = "-";
                        size_t i = 0;
                        size_t cnt = 0;
                        using boost::spirit::qi::ascii::space;

                        for (; str[i] != '\0'; i++) {
                            if (str[i] == plus_str[0]) {
                                plus++;
                            } else {
                                if (str[i] == minus_str[0]) {
                                    minus++;
                                } else {
                                    if (str[i] == mul_str[0]) {
                                        mul++;
                                    } else {
                                        if (str[i] == div_str[0]) {
                                            div++;
                                        }
                                    }
                                }
                            }
                        }

                        std::array<std::size_t, 4> ops = {plus, minus, mul, div};
                        return ops;
                        // return cnt + 1;
                    }
                    //
                    static constexpr std::size_t find_str(const char* substr, std::size_t n, std::size_t start_pos,
                                                          std::size_t end_pos) {
                        constexpr char str[sizeof...(elements) + 1] = {elements..., '\0'};

                        std::size_t size = 0;
                        for (; str[size] != '\0'; size++) {
                        }
                        size_t j = 0;
                        size_t i = start_pos;
                        if (i + n < size) {
                            for (; i < end_pos; i++) {
                                for (j = 0; j < n && str[i + j] == substr[j]; j++)
                                    ;
                                if (j == n) {
                                    return i;
                                }
                            }
                        }
                        return std::string::npos;
                    }
                    static constexpr const std::array<std::size_t, 4> ops_cnt = count_ops();

                    static constexpr const std::size_t str_len = get_str_len();

                };

            }    // namespace expressions_tstr
        }        // namespace math
    }            // namespace crypto3
}    // namespace nil
#endif    // CRYPTO3_MATH_EXPRESSION_HPP
