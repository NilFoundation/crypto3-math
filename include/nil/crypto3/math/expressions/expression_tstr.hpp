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

#ifndef CRYPTO3_MATH_EXPRESSION_HPP
#define CRYPTO3_MATH_EXPRESSION_HPP
#include <iterator>
//
//#include <nil/crypto3/math/expressions/ast.hpp>
//#include <nil/crypto3/math/expressions/evaluator.hpp>
//#include <nil/crypto3/math/expressions/parser.hpp>
//#include <nil/crypto3/math/polynomial/polynomial.hpp>
//#include <nil/crypto3/math/expressions/x3/matheval.hpp>

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

                template<char... chars>
                using tstring = std::integer_sequence<char, chars...>;
                template<typename T, T... chars>
                constexpr tstring<chars...> operator""_tstr() {
                    return {};
                }

                template<typename>
                class X;

                template<char... elements>
                class X<tstring<elements...>> {
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

                template<typename ValueType, typename ExprType>
                class ExpressionTstr {

                public:
                    static constexpr std::size_t count_str_len(const char* s) {
                        std::size_t res = 0;
                        for (; s[res] != '\0'; res++)
                            ;
                        return res;
                    }
                    using expr_type = ExprType;
                    static constexpr const char* str = expr_type::get_string();

                    static constexpr const std::size_t str_len = expr_type::get_str_len();

                    static constexpr const std::array<std::size_t, 4> ops_count = expr_type::count_ops();
                    static constexpr const std::size_t calculation_array_size =
                        ops_count[0] + ops_count[1] + ops_count[2] + +ops_count[3];
                    static constexpr const std::size_t calculation_array_size_2 = ops_count[0] + ops_count[1];

                    static constexpr const ValueType
                        eval(std::pair<std::vector<const char*>, std::vector<ValueType>> dictionary) {
                        std::array<ValueType, calculation_array_size + 1> var_array;

                        std::array<std::size_t, calculation_array_size + 1> var_pos_array;
                        std::array<std::size_t, calculation_array_size> ops_pos_array;
                        std::array<std::size_t, calculation_array_size + 1> var_index_array;
                        std::array<std::size_t, calculation_array_size> ops_index_array;
                        std::array<std::size_t, calculation_array_size_2 + 1> var_pos_array_2;
                        std::array<std::size_t, calculation_array_size_2> ops_pos_array_2;
                        std::array<ValueType, calculation_array_size_2 + 1> var_array_2;

                        for (std::size_t i = 0; i < calculation_array_size; i++) {
                            ops_index_array[i] = i;
                        }
                        for (std::size_t i = 0; i < calculation_array_size + 1; i++) {
                            var_index_array[i] = i;
                        }
                        std::size_t cnt = 0;
                        std::size_t start_pos = 0;
                        std::size_t pos = 0;
                        if (ops_count[3] > 0) {
                            std::array<std::size_t, ops_count[3]> div_pos_array;
                        }

                        std::array<const char*, 4> ops = {"+", "-", "*", "/"};
                        std::array<std::size_t, calculation_array_size> op_codes_array;
                        std::array<std::size_t, calculation_array_size_2> op_codes_array_2;
                        for (std::size_t i = 0; i < ops.size(); i++) {

                            start_pos = 0;
                            std::size_t pos = 0;
                            while (pos != std::string::npos && start_pos < str_len - 1) {
                                // std::size_t
                                pos = ExprType::find_str(ops[i], 1, start_pos, str_len);
                                if (pos != std::string::npos) {
                                    ops_pos_array[cnt] = pos;
                                    op_codes_array[cnt] = i;
                                    cnt++;
                                    start_pos = pos + 1;
                                }
                                // std::get<0>(dictionary)[i].len();
                            }
                        }
                        std::sort(ops_index_array.begin(), ops_index_array.end(),
                                  [&](const int& a, const int& b) { return (ops_pos_array[a] < ops_pos_array[b]); });
                        std::size_t end_pos = 0;
                        start_pos = 0;
                        for (std::size_t i = 0; i < calculation_array_size + 1; i++) {

                            if (i == calculation_array_size) {
                                end_pos = str_len;
                            } else {
                                end_pos = ops_pos_array[ops_index_array[i]];
                            }
                            //  std::cout <<end_pos << std::endl;
                            for (std::size_t j = 0; j < std::get<0>(dictionary).size(); j++) {
                                std::size_t name_len = count_str_len(std::get<0>(dictionary)[j]);
                                pos = ExprType::find_str(std::get<0>(dictionary)[j], name_len, start_pos, end_pos);
                                //  std::cout<<pos<<std::endl;
                                if (pos != std::string::npos) {
                                    var_pos_array[i] = pos;
                                    var_array[i] = std::get<1>(dictionary)[j];

                                    start_pos = end_pos+1;
                                    break;
                                }
                            }
                            if (pos == std::string::npos) {
                                //   std::cout << 0 << std::endl;
                                auto r = ExprType::template parse_value_type<ValueType>(start_pos, end_pos);
                                //   std::cout << r << std::endl;
                                var_pos_array[i] = pos;
                                var_array[i] = ValueType(r);
                            }
                        }

                        cnt = 0;
                        ValueType res = var_array[0];    // var_index_array[0]];

                        for (std::size_t i = 0; i < calculation_array_size; i++) {

                            if (op_codes_array[ops_index_array[i]] == 3 || op_codes_array[ops_index_array[i]] == 2) {
                                if (op_codes_array[ops_index_array[i]] == 2) {
                                    res = var_array[i] *       // var_index_array[i]] *
                                          var_array[i + 1];    // var_index_array[i + 1]];
                                } else {
                                    if (op_codes_array[ops_index_array[i]] == 3) {
                                        res = var_array[i] /       // var_index_array[i]] /
                                              var_array[i + 1];    // var_index_array[i + 1]];
                                    }
                                }
                                i++;
                                while (i < calculation_array_size) {
                                    if (op_codes_array[ops_index_array[i]] == 3) {

                                        res = res / var_array[i + 1];    // var_index_array[i + 1]];
                                        i++;
                                    } else {
                                        if (op_codes_array[ops_index_array[i]] == 2) {
                                            res = res * var_array[i + 1];    // var_index_array[i + 1]];
                                            i++;
                                        } else {
                                            break;
                                        }
                                    }
                                }
                                if (i < calculation_array_size)
                                    op_codes_array_2[cnt] = op_codes_array[ops_index_array[i]];
                                var_array_2[cnt] = res;
                                cnt++;

                            } else {
                                op_codes_array_2[cnt] = op_codes_array[ops_index_array[i]];
                                var_array_2[cnt] = var_array[i];    // var_index_array[i]];
                                cnt++;
                                if (i == calculation_array_size - 1) {
                                    var_array_2[cnt] = var_array[i + 1];    // var_index_array[i + 1]];
                                }
                            }
                        }
                        res = var_array_2[0];
                        for (std::size_t i = 0; i < calculation_array_size_2; i++) {
                            if (op_codes_array_2[i] == 0) {
                                res = res + var_array_2[i + 1];
                            } else {
                                if (op_codes_array_2[i] == 1) {
                                    res = res - var_array_2[i + 1];
                                }
                            }
                        }
                        return res;
                    }

                    constexpr static const std::size_t expr_size = expr_type::get_str_len();

                    constexpr static const std::size_t get_expr_size() {
                        return str_len;
                    }

                    constexpr static const char* get_expr() {
                        return str;
                    }

                };


//                template<typename ValueType, typename ExprType>
//                class ExpressionTstr <typename math::polynomial<ValueType>, ExprType> {//:Expression<typename math::polynomial<ValueType>, ExprType> {
//
//                public:
//                    static constexpr std::size_t count_str_len(const char* s) {
//                        std::size_t res = 0;
//                        for (; s[res] != '\0'; res++)
//                            ;
//                        return res;
//                    }
//                    using expr_type = ExprType;
//                    static constexpr const char* str = expr_type::get_string();
//
//                    static constexpr const std::size_t str_len = expr_type::get_str_len();
//
//                    static constexpr const std::array<std::size_t, 4> ops_count = expr_type::count_ops();
//                    static constexpr const std::size_t calculation_array_size =
//                        ops_count[0] + ops_count[1] + ops_count[2] + +ops_count[3];
//                    static constexpr const std::size_t calculation_array_size_2 = ops_count[0] + ops_count[1];
//
//                    static constexpr const polynomial<ValueType>
//                        eval(std::pair<std::vector<const char*>, std::vector<polynomial<ValueType>>> dictionary) {
//                        std::array<polynomial<ValueType>, calculation_array_size + 1> var_array;
//
//                        std::array<std::size_t, calculation_array_size + 1> var_pos_array;
//                        std::array<std::size_t, calculation_array_size> ops_pos_array;
//                        std::array<std::size_t, calculation_array_size + 1> var_index_array;
//                        std::array<std::size_t, calculation_array_size> ops_index_array;
//                        std::array<std::size_t, calculation_array_size_2 + 1> var_pos_array_2;
//                        std::array<std::size_t, calculation_array_size_2> ops_pos_array_2;
//                        std::array<polynomial<ValueType>, calculation_array_size_2 + 1> var_array_2;
//
//                        for (std::size_t i = 0; i < calculation_array_size; i++) {
//                            ops_index_array[i] = i;
//                        }
//                        for (std::size_t i = 0; i < calculation_array_size + 1; i++) {
//                            var_index_array[i] = i;
//                        }
//                        std::size_t cnt = 0;
//                        std::size_t start_pos = 0;
//                        std::size_t pos = 0;
//                        if (ops_count[3] > 0) {
//                            std::array<std::size_t, ops_count[3]> div_pos_array;
//                        }
//
//                        std::array<const char*, 4> ops = {"+", "-", "*", "/"};
//                        std::array<std::size_t, calculation_array_size> op_codes_array;
//                        std::array<std::size_t, calculation_array_size_2> op_codes_array_2;
//                        for (std::size_t i = 0; i < ops.size(); i++) {
//
//                            start_pos = 0;
//                            std::size_t pos = 0;
//                            while (pos != std::string::npos && start_pos < str_len - 1) {
//                                // std::size_t
//                                pos = ExprType::find_str(ops[i], 1, start_pos, str_len);
//                                if (pos != std::string::npos) {
//                                    ops_pos_array[cnt] = pos;
//                                    op_codes_array[cnt] = i;
//                                    cnt++;
//                                    start_pos = pos + 1;
//                                }
//                                // std::get<0>(dictionary)[i].len();
//                            }
//                        }
//                        std::sort(ops_index_array.begin(), ops_index_array.end(),
//                                  [&](const int& a, const int& b) { return (ops_pos_array[a] < ops_pos_array[b]); });
//                        std::size_t end_pos = 0;
//                        start_pos = 0;
//                        for (std::size_t i = 0; i < calculation_array_size + 1; i++) {
//
//                            if (i == calculation_array_size) {
//                                end_pos = str_len;
//                            } else {
//                                end_pos = ops_pos_array[ops_index_array[i]];
//                            }
//                            //  std::cout <<end_pos << std::endl;
//                            for (std::size_t j = 0; j < std::get<0>(dictionary).size(); j++) {
//                                std::size_t name_len = count_str_len(std::get<0>(dictionary)[j]);
//                                pos = ExprType::find_str(std::get<0>(dictionary)[j], name_len, start_pos, end_pos);
//                                //  std::cout<<pos<<std::endl;
//                                if (pos != std::string::npos) {
//                                    var_pos_array[i] = pos;
//                                    var_array[i] = std::get<1>(dictionary)[j];
//
//                                    start_pos = end_pos+1;
//                                    break;
//                                }
//                            }
//                            if (pos == std::string::npos) {
//                                //   std::cout << 0 << std::endl;
//                                ValueType r = ExprType::template parse_value_type<ValueType>(start_pos, end_pos);
//                                //   std::cout << r << std::endl;
//                                var_pos_array[i] = pos;
//                                var_array[i] = polynomial<ValueType>{r};
//                            }
//                        }
//
//                        cnt = 0;
//                        polynomial<ValueType> res = var_array[0];    // var_index_array[0]];
//
//                        for (std::size_t i = 0; i < calculation_array_size; i++) {
//
//                            if (op_codes_array[ops_index_array[i]] == 3 || op_codes_array[ops_index_array[i]] == 2) {
//                                if (op_codes_array[ops_index_array[i]] == 2) {
//                                    res = var_array[i] *       // var_index_array[i]] *
//                                          var_array[i + 1];    // var_index_array[i + 1]];
//                                } else {
//                                    if (op_codes_array[ops_index_array[i]] == 3) {
//                                        res = var_array[i] /       // var_index_array[i]] /
//                                              var_array[i + 1];    // var_index_array[i + 1]];
//                                    }
//                                }
//                                i++;
//                                while (i < calculation_array_size) {
//                                    if (op_codes_array[ops_index_array[i]] == 3) {
//
//                                        res = res / var_array[i + 1];    // var_index_array[i + 1]];
//                                        i++;
//                                    } else {
//                                        if (op_codes_array[ops_index_array[i]] == 2) {
//                                            res = res * var_array[i + 1];    // var_index_array[i + 1]];
//                                            i++;
//                                        } else {
//                                            break;
//                                        }
//                                    }
//                                }
//                                if (i < calculation_array_size)
//                                    op_codes_array_2[cnt] = op_codes_array[ops_index_array[i]];
//                                var_array_2[cnt] = res;
//                                cnt++;
//
//                            } else {
//                                op_codes_array_2[cnt] = op_codes_array[ops_index_array[i]];
//                                var_array_2[cnt] = var_array[i];    // var_index_array[i]];
//                                cnt++;
//                                if (i == calculation_array_size - 1) {
//                                    var_array_2[cnt] = var_array[i + 1];    // var_index_array[i + 1]];
//                                }
//                            }
//                        }
//                        res = var_array_2[0];
//                        for (std::size_t i = 0; i < calculation_array_size_2; i++) {
//                            if (op_codes_array_2[i] == 0) {
//                                res = res + var_array_2[i + 1];
//                            } else {
//                                if (op_codes_array_2[i] == 1) {
//                                    res = res - var_array_2[i + 1];
//                                }
//                            }
//                        }
//                        return res;
//                    }
//
//                    constexpr static const std::size_t expr_size = expr_type::get_str_len();
//
//                    constexpr static const std::size_t get_expr_size() {
//                        return str_len;
//                    }
//
//                    constexpr static const char* get_expr() {
//                        return str;
//                    }
//
//                };    // namespace math
//                      // namespace math
            }         // namespace expressions_tstr
        }             // namespace math
    }                 // namespace crypto3
}    // namespace nil
#endif    // CRYPTO3_MATH_EXPRESSION_HPP
