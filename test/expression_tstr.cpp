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

#define BOOST_TEST_MODULE expression_tstr_arithmetic_test

#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>

#include <boost/mpl/apply_wrap.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/bls12.hpp>

#include <nil/crypto3/math/polynomial/polynomial.hpp>
#include <nil/crypto3/math/expressions/expression_tstr.hpp>

#include <nil/crypto3/math/expressions/expression_string.hpp>
using namespace nil::crypto3::algebra;
using namespace nil::crypto3::math;
using namespace nil::crypto3::math::expressions_tstr;
typedef fields::bls12_fr<381> FieldType;

BOOST_AUTO_TEST_SUITE(expression_test_suite)
//
BOOST_AUTO_TEST_CASE(expression_expression_multiplication) {

    constexpr const char *v = "var0";
    constexpr const char *v1 = "var1";
    constexpr const char *v2 = "var2";
    constexpr const char *v3 = "var3";
    FieldType::value_type p = FieldType::value_type(5);
    FieldType::value_type p1 = FieldType::value_type(15);
    FieldType::value_type p2 = FieldType::value_type(10);
    FieldType::value_type p3 = FieldType::value_type(3);
    FieldType::value_type p4 = (p + p * p1) * (p1 - p2 + p3);

    std::vector<const char *> s = {v,v1,v2, v3};
    std::vector<FieldType::value_type> ps = {p,p1,p2,p3};

    auto dictionary = std::make_pair(s, ps);
    constexpr auto var0 = "var0 + var0 * var1"_tstr;
    constexpr auto var1 = "var1 - var2 + var3"_tstr;
    constexpr auto cc = var1.template get_tsubstring<2>();
    constexpr auto var2 = var1 * var0;
    constexpr auto var3 = var0 * var1;
    typedef ExpressionTstr <decltype(var2) >expr;
    typedef ExpressionTstr < decltype(var3) >expr2;
    auto result = evaluate<expr>(dictionary);
    auto result2 = evaluate<expr2>(dictionary);
    BOOST_CHECK_EQUAL(p4.data, result.data);
    BOOST_CHECK_EQUAL(p4.data, result2.data);

    std::cout<<std::endl;
}
BOOST_AUTO_TEST_CASE(expression_expression_addition) {

    constexpr const char *v = "var";

    FieldType::value_type p = FieldType::value_type(5);

    FieldType::value_type p2 = p + p;

    std::vector<const char *> s = {v};
    std::vector<FieldType::value_type> ps = {p};

    auto dictionary = std::make_pair(s, ps);

    constexpr auto var = "var"_tstr;
    constexpr auto var2 = var + var;

    typedef ExpressionTstr<decltype(var2)> expr;

    auto c = evaluate<expr>(dictionary);

    BOOST_CHECK_EQUAL(c.data, p2.data);

}


BOOST_AUTO_TEST_CASE(expression_expression_addition1) {
    constexpr const char *v0 = "v0";
    constexpr const char *v1 = "v1";
    constexpr const char *v2 = "v2";
    constexpr const char *v3 = "v3";

    FieldType::value_type p0 = FieldType::value_type(5);
    FieldType::value_type p1 = FieldType::value_type(15);
    FieldType::value_type p2 = FieldType::value_type(5);
    FieldType::value_type p3 = FieldType::value_type(15);

    FieldType::value_type p4 = p0 + p1 + p2 * p3;

    std::vector<const char *> s = {v0, v1, v2, v3};
    std::vector<FieldType::value_type> ps = {p0, p1, p2, p3};

    auto dictionary = std::make_pair(s, ps);

    constexpr auto var0 = "v0 + v1"_tstr;
    constexpr auto var1 = "v2 * v3"_tstr;
    constexpr auto var2 = var0 + var1;

    typedef ExpressionTstr<decltype(var2)> expr;

    FieldType::value_type c = evaluate<expr>(dictionary);

    BOOST_CHECK_EQUAL(c.data, p4.data);

}
 BOOST_AUTO_TEST_CASE(expression_expression_evaluate) {

     constexpr const char *v1 = "v1";
     constexpr const char *v0 = "v0";

     FieldType::value_type p0 = FieldType::value_type(5);
     FieldType::value_type p1 = FieldType::value_type(15);

     FieldType::value_type p2 = p0 + p1;

     std::vector<const char *> s = {v0, v1};
     std::vector<FieldType::value_type> ps = {p0, p1};

     auto dictionary = std::make_pair(s, ps);
constexpr auto var = "v0 + v1"_tstr;
     typedef ExpressionTstr<decltype(var)> expr;

     FieldType::value_type c = evaluate<expr>(dictionary);

     BOOST_CHECK_EQUAL(c.data, p2.data);

 }

 BOOST_AUTO_TEST_CASE(expression_expression_0) {

     constexpr const char *v1 = "v1";
     constexpr const char *v0 = "v0";

     FieldType::value_type p0 = FieldType::value_type(5);
     FieldType::value_type p1 = FieldType::value_type(15);

     FieldType::value_type p2 = p0 - p1;

     std::vector<const char *> s = {v0, v1};
     std::vector<FieldType::value_type> ps = {p0, p1};

     auto dictionary = std::make_pair(s, ps);
     auto var = "v0 - v1"_tstr;
     typedef ExpressionTstr<decltype(var)> expr;

     FieldType::value_type c = evaluate<expr>(dictionary);

     BOOST_CHECK_EQUAL(c.data, p2.data);

 }

 BOOST_AUTO_TEST_CASE(expression_expression_1) {

     constexpr const char *v1 = "v1";
     constexpr const char *v0 = "v0";

     FieldType::value_type p0 = FieldType::value_type(5);
     FieldType::value_type p1 = FieldType::value_type(15);

     FieldType::value_type p2 = p0 * p1;

     std::vector<const char *> s = {v0, v1};
     std::vector<FieldType::value_type> ps = {p0, p1};

     auto dictionary = std::make_pair(s, ps);
     auto var = "v0 * v1"_tstr;
     typedef ExpressionTstr<decltype(var)> expr;

     FieldType::value_type c = evaluate<expr>(dictionary);

     BOOST_CHECK_EQUAL(c.data, p2.data);

 }

 BOOST_AUTO_TEST_CASE(expression_expression_2) {

     constexpr const char *v1 = "v1";
     constexpr const char *v0 = "v0";

     FieldType::value_type p0 = FieldType::value_type(5);
     FieldType::value_type p1 = FieldType::value_type(15);

     FieldType::value_type p2 = p1 / p0;

     std::vector<const char *> s = {v0, v1};
     std::vector<FieldType::value_type> ps = {p0, p1};

     auto dictionary = std::make_pair(s, ps);
     auto var = "v1 / v0"_tstr;
     typedef ExpressionTstr<decltype(var)> expr;

     FieldType::value_type c = evaluate<expr>(dictionary);

     BOOST_CHECK_EQUAL(c.data, p2.data);

 }

 BOOST_AUTO_TEST_CASE(expression_polynomial_expression_1) {
     //
     constexpr const char *v1 = "v1";
     constexpr const char *v0 = "v0";
     //
     polynomial<FieldType::value_type> p0 = {5, 0, 0, 13, 0, 1};
     polynomial<FieldType::value_type> p1 = {13, 0, 1};
     // auto x =p0.evaluate(FieldType::value_type);
     // std::cout<<x.data<<std::endl;
     polynomial<FieldType::value_type> p2;
     p2 = p0 + p1 / p0;
     auto var = "v0 + v1 / v0"_tstr;
     typedef ExpressionTstr<decltype(var)> expr;


     std::vector<const char *> s = {v0, v1};
     std::vector<polynomial<FieldType::value_type>> ps = {p0, p1};

     auto dictionary = std::make_pair(s, ps);

     auto res = evaluate<expr>(dictionary);


     for (std::size_t i = 0; i < res.size(); i++) {
         BOOST_CHECK_EQUAL(res[i].data, p2[i].data);
     }

     FieldType::value_type val0 = FieldType::value_type(5);
     FieldType::value_type val1 = FieldType::value_type(15);
     auto val2 = val0 + val1 / val0;
     std::vector<FieldType::value_type> vals = {val0, val1};

     auto dictionary2 = std::make_pair(s, vals);

     auto res2 = evaluate<expr>(dictionary2);

     BOOST_CHECK_EQUAL(res2.data, val2.data);
 }


BOOST_AUTO_TEST_SUITE_END()