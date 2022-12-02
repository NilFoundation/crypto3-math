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

                template<char... Chars1, char... Chars2>
                constexpr tstring<Chars1..., Chars2...> concat(tstring<Chars1...>, tstring<Chars2...>) {
                    return tstring<Chars1..., Chars2...>();
                }
                template<typename T, T... Chars1, T... Chars2>
                constexpr auto operator-(tstring<Chars1...>, tstring<Chars2...>) {
                    if constexpr (sizeof...(Chars2) > 0)
                        return tstring<Chars1..., ' ', '-', ' ', Chars2...>();
                    else
                        return tstring<Chars1...>();
                }
                template<typename T, T... Chars1, T... Chars2>
                constexpr auto operator+(tstring<Chars1...>, tstring<Chars2...>) {
                    if constexpr (sizeof...(Chars2) > 0)
                        return tstring<Chars1..., ' ', '+', ' ', Chars2...>();
                    else
                        return tstring<Chars1...>();
                }

                template<typename T, T... Chars1, T... Chars2>
                constexpr tstring<Chars1..., ' ', '*', ' ', Chars2...> operator*(tstring<Chars1...>,
                                                                                 tstring<Chars2...>) {
                    return tstring<Chars1..., ' ', '*', ' ', Chars2...>();
                }

                template<typename>
                class X;

                template<char... elements>
                class X<tstring<elements...>> {
                public:
                    static constexpr const size_t size = sizeof...(elements) + 1;
                    static constexpr const std::array<char, size> s = {elements..., '\0'};
                    static constexpr const size_t pluses = static_string_count(s, '+', 0);
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

                    template<typename ValueType>
                    static constexpr ValueType parse_value_type(std::size_t start, std::size_t end) {
                        constexpr const char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        const char* space = " ";
                        //                        repeated<one_char>
                        std::size_t cnt = -1;
                        ValueType res = 0;
                        for (std::size_t i = start; i < end; i++) {
                            if (str[i] != space[0]) {
                                cnt += 1;
                            }
                        }
                        for (std::size_t i = start; i < end; i++) {
                            if (str[i] != space[0]) {
                                res +=
                                    ValueType((int)(str[i]) - (int)('0')) *
                                    ValueType(10).pow(
                                        cnt);    //(int)(pow(10,cnt)) ;
                                                 //    std::cout<<ValueType((int)(str[i])-(int)('0'))*ValueType(10).pow(cnt)<<std::endl;//((int)(str[i])-(int)('0'))*(int)(pow(10,cnt))<<std::endl;
                                cnt -= 1;
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
                    static constexpr const std::size_t N = ops_cnt[0] + ops_cnt[1] + 1;
                    static constexpr std::array<std::size_t, N> get_substr() {
                        std::array<std::size_t, N> arr = {};
                        std::size_t start_pos = 0;
                        std::size_t pos = 0;
                        std::size_t cnt = 0;
                        while (cnt < N - 1) {
                            // std::size_t
                            pos = std::min(find_str("+", 1, start_pos, str_len), find_str("-", 1, start_pos, str_len));
                            if (pos != std::string::npos) {

                                arr[cnt] = pos - start_pos - 1;
                                // op_codes_array1[cnt] = i;
                                cnt++;
                                start_pos = pos + 2;
                            }

                            // std::get<0>(dictionary)[i].len();
                        }
                        arr[cnt] = str_len - start_pos;

                        return arr;
                    }
                    template<size_t i>
                    static constexpr bool get_substr_sign() {
                        if constexpr (i < N - 1) {
                            std::size_t start_pos = 0;
                            std::size_t pos = 0;
                            std::size_t cnt = 0;
                            while (cnt < i) {
                                // std::size_t
                                pos = std::min(find_str("+", 1, start_pos, str_len),
                                               find_str("-", 1, start_pos, str_len));
                                if (pos != std::string::npos) {

                                    // arr[cnt] = pos - start_pos - 1;
                                    // op_codes_array1[cnt] = i;
                                    cnt++;
                                    start_pos = pos + 2;
                                }
                            }
                            if (find_str("+", 1, start_pos, str_len) < find_str("-", 1, start_pos, str_len)) {
                                return true;
                            } else {
                                return false;
                            }
                        }
                    }
                    static constexpr size_t get_substr_sum() {
                        std::size_t sum = 0;
                        std::size_t start_pos = 0;
                        std::size_t pos = 0;
                        std::size_t cnt = 0;
                        while (cnt < N - 1) {
                            // std::size_t
                            pos = std::min(find_str("+", 1, start_pos, str_len), find_str("-", 1, start_pos, str_len));
                            if (pos != std::string::npos) {

                                sum = sum + pos - start_pos - 1;
                                // op_codes_array1[cnt] = i;
                                cnt++;
                                start_pos = pos + 2;
                            }

                            // std::get<0>(dictionary)[i].len();
                        }
                        sum = sum + str_len - start_pos;

                        return sum;
                    }

                    template<size_t i>
                    static constexpr auto get_char_s() {
                        if constexpr (i < size) {
                            return tstring<s[i]>();
                        }
                    }
                    static constexpr std::array<std::size_t, N> get_substr_pos() {
                        std::array<std::size_t, N> arr = {};
                        std::size_t start_pos = 0;
                        std::size_t pos = 0;
                        std::size_t cnt = 1;
                        arr[0] = 0;
                        while (cnt < N) {
                            // std::size_t
                            pos = std::min(find_str("+", 1, start_pos, str_len), find_str("-", 1, start_pos, str_len));
                            if (pos != std::string::npos) {

                                // op_codes_array1[cnt] = i;

                                start_pos = pos + 2;
                                arr[cnt] = start_pos;
                                cnt++;
                            }

                            // std::get<0>(dictionary)[i].len();
                        }

                        return arr;
                    }
                    template<size_t i, size_t end, char... Chars>
                    static constexpr auto get_tstring(tstring<Chars...>&&) {
                        constexpr const std::array<std::size_t, N> substrs = get_substr();
                        if constexpr (i < end) {
                            //<Chars..., get_char_s<i>()>();
                            //  std::cout<<"get_tstring i "<<i<<std::endl;
                            //   auto res =
                            return get_tstring<i + 1, end>(concat(tstring<Chars...>(), get_char_s<i>()));
                        } else {
                            //   std::cout<<"get_tstring else i "<<i<<std::endl;
                            return tstring<Chars...>();
                        }
                        //   std::cout<<"noreturn"<<std::endl;

                        // return X<decltype(res)>();
                    }
                    template<size_t i>
                    static constexpr auto get_tsubstring() {
                        constexpr const std::array<std::size_t, N> substrs = get_substr();
                        constexpr const std::array<std::size_t, N> substrs_pos = get_substr_pos();
                        if constexpr (i < N) {
                            //    if constexpr (i == 0) {
                            // std::cout<<"gettsubstring i "<<i<<" substrs[i] "<<substrs[i]<<std::endl;
                            return get_tstring<substrs_pos[i], substrs_pos[i] + substrs[i]>(tstring<>());
                            // return substr;
                            //                            } else {
                            //                                //     std::cout<<"gettsubstring i "<<i<<" substrs[i]
                            //                                "<<substrs[i]<<std::endl; return get_tstring<substrs[i -
                            //                                1] + 3, substrs[i - 1] + 3 + substrs[i]>(tstring<>());
                            //                            }
                        }

                        // return X<decltype(res)>();
                    }
                    template<typename T>
                    static constexpr auto get_i() {
                        //   typedef std::integral_constant<size_t,i>;
                        // std::integral_constant<i,size_t>;
                        return T();
                    }
                    static constexpr auto get_char_s1(size_t i) {
                        constexpr const size_t j = get_i<decltype(i)>();
                        return X<decltype(tstring<s[j]>())>();
                    }
                    template<size_t i>
                    static constexpr size_t get_size_sub() {
                        constexpr std::array<std::size_t, N> substrs = get_substr();
                        return substrs[i];
                    }
                    template<char... Chars1>
                    constexpr auto operator=(X<tstring<Chars1...>>) {
                        //                    X<decltype(tstring<Chars1...>() + tstring<Chars2...>())> x;
                        return X<tstring<Chars1...>>();
                    }

                };

                template<char... elements>
                class X<const std::integer_sequence<char, elements...>> {
                public:
                    // X(){}/
                    static constexpr const size_t size = sizeof...(elements) + 1;
                    static constexpr const std::array<char, sizeof...(elements) + 1> s = {elements..., '\0'};

                    static constexpr const char* get_string() {
                        constexpr const char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        return str;
                    }

                    template<typename ValueType>
                    static constexpr ValueType parse_value_type(std::size_t start, std::size_t end) {
                        constexpr const char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        const char* space = " ";
                        //                        repeated<one_char>
                        std::size_t cnt = -1;
                        ValueType res = 0;
                        for (std::size_t i = start; i < end; i++) {
                            if (str[i] != space[0]) {
                                cnt += 1;
                            }
                        }
                        for (std::size_t i = start; i < end; i++) {
                            if (str[i] != space[0]) {
                                res +=
                                    ValueType((int)(str[i]) - (int)('0')) *
                                    ValueType(10).pow(
                                        cnt);    //(int)(pow(10,cnt)) ;
                                                 //    std::cout<<ValueType((int)(str[i])-(int)('0'))*ValueType(10).pow(cnt)<<std::endl;//((int)(str[i])-(int)('0'))*(int)(pow(10,cnt))<<std::endl;
                                cnt -= 1;
                            }
                        }
                        return res;
                    }
                    static constexpr const char* str = get_string();

                    constexpr static const std::size_t get_str_len() {
                        //                        constexpr char str[sizeof...(elements) + 1] = {elements..., '\0'};
                        //
                        //                        std::size_t size = 0;
                        //                        for (; str[size] != '\0'; size++) {
                        //                        }
                        return sizeof...(elements) + 1;
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

                    static constexpr const std::size_t N = ops_cnt[0] + ops_cnt[1];

                };

                template<char... Chars1, char... Chars2>
                constexpr auto operator+(X<tstring<Chars1...>>, X<tstring<Chars2...>>) {
                    //                    X<decltype(tstring<Chars1...>() + tstring<Chars2...>())> x;
                    return    // tstring<Chars1...>() + tstring<Chars2...>();//
                        X<decltype(tstring<Chars1...>() + tstring<Chars2...>())>();
                }
                template<char... Chars1, char... Chars2>
                constexpr auto operator-(X<tstring<Chars1...>>, X<tstring<Chars2...>>) {
                    return X<decltype(tstring<Chars1...>() - tstring<Chars2...>())>();
                    // return tstring<Chars1...>() - tstring<Chars2...>();
                }

                template<typename T, T... Chars1, T... Chars2>
                constexpr X<std::integer_sequence<char, Chars1..., Chars2...>>
                    operator|(X<std::integer_sequence<char, Chars1...>>, X<std::integer_sequence<char, Chars2...>>) {
                    return X<std::integer_sequence<char, Chars1..., Chars2...>>();
                }


                template<size_t N, char... Chars>
                constexpr const std::array<std::size_t, N> get_substr() {
                    size_t str_len = sizeof...(Chars);
                    std::array<std::size_t, N> arr;
                    std::size_t start_pos = 0;
                    std::size_t pos = 0;
                    std::size_t cnt = 0;
                    while (cnt < N - 1) {
                        // std::size_t
                        pos = std::min(X<tstring<Chars...>>::find_str("+", 1, start_pos, str_len),
                                       X<tstring<Chars...>>::find_str("-", 1, start_pos, str_len));
                        if (pos != std::string::npos) {

                            arr[cnt] = pos - start_pos - 1;
                            // op_codes_array1[cnt] = i;
                            cnt++;
                            start_pos = pos + 2;
                        }

                        // std::get<0>(dictionary)[i].len();
                    }
                    arr[cnt] = str_len - start_pos;
                    return arr;
                }

                template<char... Chars1, char... Chars2>
                constexpr const size_t get_mul_len() {
                    constexpr const char str1[sizeof...(Chars1) + 1] = {Chars1..., '\0'};
                    constexpr const char str2[sizeof...(Chars2) + 1] = {Chars2..., '\0'};
                    constexpr const size_t size1 = sizeof...(Chars1);
                    constexpr const size_t size2 = sizeof...(Chars2);
                    using ExprType1 = X<tstring<Chars1...>>;
                    using ExprType2 = X<tstring<Chars2...>>;
                    constexpr const std::array<size_t, 4> ops_count1 = ExprType1::count_ops();
                    constexpr const std::array<size_t, 4> ops_count2 = ExprType2::count_ops();
                    constexpr const std::size_t calculation_array_size_1 = ops_count1[0] + ops_count1[1];
                    constexpr const std::size_t calculation_array_size_2 = ops_count2[0] + ops_count2[1];

                    std::array<std::size_t, calculation_array_size_1 + 1> arr;
                    std::size_t start_pos = 0;
                    std::size_t pos = 0;
                    std::size_t cnt = 0;
                    while (cnt < calculation_array_size_1) {
                        // std::size_t
                        pos = std::min(ExprType1::find_str("+", 1, start_pos, ExprType1::str_len),
                                       ExprType1::find_str("-", 1, start_pos, ExprType1::str_len));
                        if (pos != std::string::npos) {

                            arr[cnt] = pos - start_pos - 1;
                            // op_codes_array1[cnt] = i;
                            cnt++;
                            start_pos = pos + 2;
                        }

                        // std::get<0>(dictionary)[i].len();
                    }
                    arr[cnt] = ExprType1::str_len - start_pos;
                    auto arr1 = ExprType1::get_substr();
                    auto arr2 =
                        ExprType2::get_substr();    // get_substr<calculation_array_size_1 + 1,size1,Chars2...>();

                    size_t newlen =
                        ((ops_count2[0] + ops_count1[1] + 1) * (ops_count2[0] + ops_count1[1] + 1) - 1 +
                         (ops_count2[0] + ops_count1[1] + 1) * (ops_count2[0] + ops_count1[1] + 1) +
                         (ops_count2[0] + ops_count1[1] + 1) * arr2[0] + (ops_count1[0] + ops_count1[1] + 1) * arr2[1] +
                         (ops_count2[0] + ops_count2[1] + 1) * arr1[0] +
                         (ops_count2[0] + ops_count2[1] + 1) * arr1[1]) +
                        ((ops_count2[0] + ops_count1[1] + 1) * (ops_count2[0] + ops_count1[1] + 1) - 1 +
                         (ops_count2[0] + ops_count1[1] + 1) * (ops_count2[0] + ops_count1[1] + 1)) *
                            2;

                    return newlen;
                }

                template<typename T, T... chars>
                constexpr X<tstring<chars...>> operator""_tstr() {
                    //                    std::integral_constant<tstring<
                    //    tstring<chars...> x();
                    // using y = X<tstring<chars...>>;
                    return X<tstring<chars...>>();
                }

                template<bool sign, size_t cnt, size_t i, size_t end1, size_t j, size_t end2, char... Chars1,
                         char... Chars2>
                constexpr auto sumprod(X<tstring<Chars1...>> x1, X<tstring<Chars2...>> x2) {
                    if constexpr (end2 == 1) {
                        return X<tstring<Chars1...>>::template get_tsubstring<i>() *
                               X<tstring<Chars2...>>::template get_tsubstring<j>();
                    }
                    if constexpr (end2 == 2) {
                        if constexpr ((sign == true && X<tstring<Chars2...>>::template get_substr_sign<j>() == true) ||
                                      (sign == false &&
                                       X<tstring<Chars2...>>::template get_substr_sign<j>() == false)) {
                            return X<tstring<Chars1...>>::template get_tsubstring<i>() *
                                       X<tstring<Chars2...>>::template get_tsubstring<j>() +
                                   X<tstring<Chars1...>>::template get_tsubstring<i>() *
                                       X<tstring<Chars2...>>::template get_tsubstring<j + 1>();
                        } else {
                            return X<tstring<Chars1...>>::template get_tsubstring<i>() *
                                       X<tstring<Chars2...>>::template get_tsubstring<j>() -
                                   X<tstring<Chars1...>>::template get_tsubstring<i>() *
                                       X<tstring<Chars2...>>::template get_tsubstring<j + 1>();
                        }
                    }

                    if constexpr (j < end2 - 1) {
                        if constexpr (j > 0) {
                            if constexpr ((sign == true &&
                                           X<tstring<Chars2...>>::template get_substr_sign<j>() == true) ||
                                          (sign == false &&
                                           X<tstring<Chars2...>>::template get_substr_sign<j>() == false)) {

                                return X<tstring<Chars1...>>::template get_tsubstring<i>() *
                                           X<tstring<Chars2...>>::template get_tsubstring<j>() +
                                       sumprod<sign, cnt + 1, i, end1, j + 1, end2>(X<tstring<Chars1...>>(),
                                                                                    X<tstring<Chars2...>>());
                            } else {
                                return X<tstring<Chars1...>>::template get_tsubstring<i>() *
                                           X<tstring<Chars2...>>::template get_tsubstring<j>() -
                                       sumprod<sign, cnt + 1, i, end1, j + 1, end2>(X<tstring<Chars1...>>(),
                                                                                    X<tstring<Chars2...>>());
                            }
                        } else {
                            if constexpr (j == 0) {
                                if constexpr ((sign == true &&
                                               X<tstring<Chars2...>>::template get_substr_sign<j>() == true) ||
                                              (sign == false &&
                                               X<tstring<Chars2...>>::template get_substr_sign<j>() == false)) {
                                    return X<tstring<Chars1...>>::template get_tsubstring<i>() *
                                               X<tstring<Chars2...>>::template get_tsubstring<j>() +
                                           sumprod<sign, cnt + 1, i, end1, j + 1, end2>(X<tstring<Chars1...>>(),
                                                                                        X<tstring<Chars2...>>());
                                } else {
                                    return X<tstring<Chars1...>>::template get_tsubstring<i>() *
                                               X<tstring<Chars2...>>::template get_tsubstring<j>() -
                                           sumprod<sign, cnt + 1, i, end1, j + 1, end2>(X<tstring<Chars1...>>(),
                                                                                        X<tstring<Chars2...>>());
                                    //                                            }
                                }
                            }
                        }
                    }

                    else {
                        return X<tstring<Chars1...>>::template get_tsubstring<i>() *
                               X<tstring<Chars2...>>::template get_tsubstring<j>();
                    }
                }


                template<size_t cnt, size_t i, size_t end1, char... Chars1, char... Chars2>
                constexpr auto sumprod(X<tstring<Chars1...>> x1, X<tstring<Chars2...>> x2) {
                    if constexpr (end1 == 1) {
                        return sumprod<true, cnt, 0, end1, 0, X<tstring<Chars2...>>::N>(X<tstring<Chars1...>>(),
                                                                                        X<tstring<Chars2...>>());
                    }

                    if constexpr (i < end1 - 2) {
                        if constexpr (i == 0) {
                            if constexpr (X<tstring<Chars1...>>::template get_substr_sign<i>() == true) {
                                return sumprod<true, cnt, i, end1, 0, X<tstring<Chars2...>>::N>(
                                           X<tstring<Chars1...>>(), X<tstring<Chars2...>>()) +
                                       sumprod<cnt, i + 1, end1>(X<tstring<Chars1...>>(), X<tstring<Chars2...>>());
                            } else {
                                return sumprod<false, cnt, i, end1, 0, X<tstring<Chars2...>>::N>(
                                           X<tstring<Chars1...>>(), X<tstring<Chars2...>>()) -
                                       sumprod<cnt, i + 1, end1>(X<tstring<Chars1...>>(), X<tstring<Chars2...>>());
                            }
                        } else {

                            if constexpr (X<tstring<Chars1...>>::template get_substr_sign<i>() == true) {
                                return sumprod<true, cnt, i, end1, 0, X<tstring<Chars2...>>::N>(
                                           X<tstring<Chars1...>>(), X<tstring<Chars2...>>()) +
                                       sumprod<cnt, i + 1, end1>(X<tstring<Chars1...>>(), X<tstring<Chars2...>>());
                            } else {
                                return sumprod<false, cnt, i, end1, 0, X<tstring<Chars2...>>::N>(
                                           X<tstring<Chars1...>>(), X<tstring<Chars2...>>()) -
                                       sumprod<cnt, i + 1, end1>(X<tstring<Chars1...>>(), X<tstring<Chars2...>>());
                            }

                            //    }
                        }
                    } else {
                        if constexpr (i == end1 - 2) {
                            if constexpr (X<tstring<Chars1...>>::template get_substr_sign<i>() == true) {
                                return sumprod<true, cnt, i, end1, 0, X<tstring<Chars2...>>::N>(
                                           X<tstring<Chars1...>>(), X<tstring<Chars2...>>()) +
                                       sumprod<true, cnt, i + 1, end1, 0, X<tstring<Chars2...>>::N>(
                                           X<tstring<Chars1...>>(), X<tstring<Chars2...>>());
                            } else {
                                return sumprod<true, cnt, i, end1, 0, X<tstring<Chars2...>>::N>(
                                           X<tstring<Chars1...>>(), X<tstring<Chars2...>>()) -
                                       sumprod<false, cnt, i + 1, end1, 0, X<tstring<Chars2...>>::N>(
                                           X<tstring<Chars1...>>(), X<tstring<Chars2...>>());
                            }
                        }
                    }

                }

                template<typename T, T... Chars1, T... Chars2>
                constexpr auto
                    operator*(X<std::integer_sequence<T, Chars1...>> x1, X<std::integer_sequence<T, Chars2...>> x2) {

                    using ExprType1 = X<tstring<Chars1...>>;
                    using ExprType2 = X<tstring<Chars2...>>;

                    if constexpr (ExprType1::N <= ExprType2::N) {
                        auto res = sumprod<0, 0, ExprType1::N>(
                            ExprType1(),
                            ExprType2());    // + sumprod<0,ExprType1::N, 1,ExprType2::N>(ExprType1(),ExprType2());

                        return X<decltype(res)>();    // ExprType1::template get_char_s<0>()|ExprType1::template
                                                      // get_char_s<1>();
                    } else {
                        auto res = sumprod<0, 0, ExprType2::N>(
                            ExprType2(),
                            ExprType1());    // + sumprod<0,ExprType1::N, 1,ExprType2::N>(ExprType1(),ExprType2());

                        return X<decltype(res)>();
                    }

                }
            }    // namespace expressions_tstr
        }        // namespace math
    }            // namespace crypto3
}    // namespace nil
#endif    // CRYPTO3_MATH_EXPRESSION_HPP
