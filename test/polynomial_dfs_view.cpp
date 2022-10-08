//---------------------------------------------------------------------------//
// Copyright (c) 2020-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2021 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@nil.foundation>
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

#define BOOST_TEST_MODULE polynomial_dfs_view_test

#include <vector>
#include <cstdint>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <nil/crypto3/algebra/fields/arithmetic_params/bls12.hpp>

#include <nil/crypto3/math/polynomial/polynomial_dfs_view.hpp>

using namespace nil::crypto3::algebra;
using namespace nil::crypto3::math;

typedef fields::bls12_fr<381> FieldType;

BOOST_AUTO_TEST_SUITE(polynomial_dfs_view_from_coefficients_test_suite)

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_from_coefficients_less_degree) {
    std::vector<typename FieldType::value_type> a_v;
    std::vector<typename FieldType::value_type> polynomial = {1, 3, 4, 25, 6, 7, 7};

    polynomial_dfs_view<typename FieldType::value_type> a = {0, a_v};
    a.from_coefficients(polynomial);

    std::vector<typename FieldType::value_type> c_res = {
        0x35_cppui253,
        0x26D37C08AED60085FDE335498E7DFEE2AFB1463D06E338219CD0E5DDAF27D68F_cppui253,
        0x73EDA753299D7D3FEB6ED7EF1F748FC77F90A3DE15D15BFEFFF0FFFEFFFFFFFD_cppui253,
        0x4871BC0D4FC8E6B9695B3B2BDCA6D2CACD64A30E404507B3A523C00D0FF4F223_cppui253,
        0x73EDA753299D7D483339D80809A1D80553BDA402FFFE5BFEFFFFFFFEFFFFFFF0_cppui253,
        0x4D1A2B4A7AC77CBEE56BD5E7B711BC3D1BFA5DB7350923DD63291A2150D82968_cppui253,
        0x847CB0018EA2D483DD42D0024EA2D0000000EFFFFFFFFFFFC_cppui253,
        0x2B7BEB45D9D4969219C969B2F10D22200E6B010383CB544B5AE23FF1F00B0DD4_cppui253};

    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(polynomial_dfs_view_coefficients_test_suite)

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_coefficients_less_degree) {
    std::vector<typename FieldType::value_type> a_v = {
        0x35_cppui253,
        0x26D37C08AED60085FDE335498E7DFEE2AFB1463D06E338219CD0E5DDAF27D68F_cppui253,
        0x73EDA753299D7D3FEB6ED7EF1F748FC77F90A3DE15D15BFEFFF0FFFEFFFFFFFD_cppui253,
        0x4871BC0D4FC8E6B9695B3B2BDCA6D2CACD64A30E404507B3A523C00D0FF4F223_cppui253,
        0x73EDA753299D7D483339D80809A1D80553BDA402FFFE5BFEFFFFFFFEFFFFFFF0_cppui253,
        0x4D1A2B4A7AC77CBEE56BD5E7B711BC3D1BFA5DB7350923DD63291A2150D82968_cppui253,
        0x847CB0018EA2D483DD42D0024EA2D0000000EFFFFFFFFFFFC_cppui253,
        0x2B7BEB45D9D4969219C969B2F10D22200E6B010383CB544B5AE23FF1F00B0DD4_cppui253};

    polynomial_dfs_view<typename FieldType::value_type> a = {7, a_v};
    std::vector<typename FieldType::value_type> c_res = {1, 3, 4, 25, 6, 7, 7};
    std::vector<typename FieldType::value_type> c = a.coefficients();

    BOOST_CHECK_EQUAL(c_res.size(), c.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, c[i].data);
    }
}

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_coefficients_same_degree) {
    std::vector<typename FieldType::value_type> a_v = {
        0x37,
        0x6C17ABF513DFFC886A7F49F970801792C825CFDD829870DC60E8DA51F53633_cppui253,
        0x73EDA753299D7D3ED0CB3E52336E8625A78AA3D929CB5BFEFFEEFFFEFFFFFFFD_cppui253,
        0x53B09574717196328488C7990499B10ABA0C038C321BF5B1C0D1C5A4E10C7330_cppui253,
        0x73EDA753299D7D483339D80809A1D80553BDA402FFFE5BFEFFFFFFFEFFFFFFEE_cppui253,
        0x73818FA734899D485AE48BE74C1F3B0838E37E245E69C38E23991724AE0AC9C4_cppui253,
        0x9626E99B5D63351DFAC330029D63300000010FFFFFFFFFFFC_cppui253,
        0x203D11DEB82BE718FE9BDD45C91A43E021C3A08591F4664D3F343A5A1EF38CC7_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> a = {8, a_v};
    std::vector<typename FieldType::value_type> c_res = {1, 3, 4, 25, 6, 7, 7, 2};
    std::vector<typename FieldType::value_type> c = a.coefficients();

    BOOST_CHECK_EQUAL(c_res.size(), c.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, c[i].data);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(polynomial_dfs_view_addition_test_suite)

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_addition_equal) {
    std::vector<typename FieldType::value_type> a_v = {
        0x37_cppui253,
        0x6C17ABF513DFFC886A7F49F970801792C825CFDD829870DC60E8DA51F53633_cppui253,
        0x73EDA753299D7D3ED0CB3E52336E8625A78AA3D929CB5BFEFFEEFFFEFFFFFFFD_cppui253,
        0x53B09574717196328488C7990499B10ABA0C038C321BF5B1C0D1C5A4E10C7330_cppui253,
        0x73EDA753299D7D483339D80809A1D80553BDA402FFFE5BFEFFFFFFFEFFFFFFEE_cppui253,
        0x73818FA734899D485AE48BE74C1F3B0838E37E245E69C38E23991724AE0AC9C4_cppui253,
        0x9626E99B5D63351DFAC330029D63300000010FFFFFFFFFFFC_cppui253,
        0x203D11DEB82BE718FE9BDD45C91A43E021C3A08591F4664D3F343A5A1EF38CC7_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> a = {7, a_v};

    std::vector<typename FieldType::value_type> b_v = {
        0x3a_cppui253,
        0x67f753af0b9db226952762685b47bb06f22600d84e5cf11425a7f17246806009_cppui253,
        0x73eda753299d7d3e43797183bd6b8154bb87a3d6b3c85bfeffedffff00000001_cppui253,
        0x4aad82754c8121ad40543e15922c725cee0fee48fc9d5b66d5fec3be9ec1d37_cppui253,
        0x6_cppui253,
        0xbf653a41dffcb283de80f4d367e56c971bba34839c56aeada640e8cb97f9ffc_cppui253,
        0x9efc066844c3656b09836002c4c3600000012000000000000_cppui253,
        0x6f42cf2bd4d56b26bf5efa79285ad71474b8a500e8108648929413c31613e2ce_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> b = {7, b_v};

    a += b;

    std::vector<typename FieldType::value_type> c_v = {
        0x71_cppui253,
        0x68636b5b00b192231d91e1b254b83b1e84ee26a82bdf89850208da4c9875963c_cppui253,
        0x73eda753299d7d34e10ad7cde7382f750f54a3acdd955bfeffdcfffefffffffd_cppui253,
        0x585b6d9bc639a84d588e0b7a5dbc783088ed0270c1e5cb682e31b1e0caf89067_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffff4_cppui253,
        0xb8a3bf828ebeb286592c32c78fbb9cc56e17d699830d279fdfd25b2678a69bf_cppui253,
        0x13522f003a2269a89044690056226900000022fffffffffffc_cppui253,
        0x1b9239b76363d4f78ac0ffb6e7d342ef42bea1837a069096d1c84e1e35076f94_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> c_res = {7, c_v};
    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(c_res.degree(), a.degree());
}

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_addition_less_b) {
    std::vector<typename FieldType::value_type> a_v = {
        0x37_cppui253,
        0x6C17ABF513DFFC886A7F49F970801792C825CFDD829870DC60E8DA51F53633_cppui253,
        0x73EDA753299D7D3ED0CB3E52336E8625A78AA3D929CB5BFEFFEEFFFEFFFFFFFD_cppui253,
        0x53B09574717196328488C7990499B10ABA0C038C321BF5B1C0D1C5A4E10C7330_cppui253,
        0x73EDA753299D7D483339D80809A1D80553BDA402FFFE5BFEFFFFFFFEFFFFFFEE_cppui253,
        0x73818FA734899D485AE48BE74C1F3B0838E37E245E69C38E23991724AE0AC9C4_cppui253,
        0x9626E99B5D63351DFAC330029D63300000010FFFFFFFFFFFC_cppui253,
        0x203D11DEB82BE718FE9BDD45C91A43E021C3A08591F4664D3F343A5A1EF38CC7_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> a = {7, a_v};

    std::vector<typename FieldType::value_type> b_v = {
        0x17_cppui253, 0x1a7f5666b62090e72c4090007620900000002fffffffffffe_cppui253, 0x11_cppui253,
        0x73eda753299d7d468b44719ca798c9928fb4a3fb9df55bfefffcfffeffffffff_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> b = {2, b_v};

    a += b;

    std::vector<typename FieldType::value_type> c_v = {
        0x4e_cppui253,
        0x2984a53ad76597710bc6e589547653b47dcce72f84e0617332e6e0761851f4a9_cppui253,
        0x73eda753299d7d4078c0a4bd957794986b93a3e08bd45bfefff1fffefffffffb_cppui253,
        0x195e04ac5e7749b26f0033b1486ae23bed8b1011de0d893be16ec12aecd863c2_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffff_cppui253,
        0x4a6902185237e5dffc8fbf66155bd15f9620bcfadb4dfa8bcd291f88e7ae0b60_cppui253,
        0x7ba79334a742a436ce82a0022742a0000000dfffffffffffa_cppui253,
        0x5a8fa2a6cb26338cef1cd76f6106a8baa60293c9c1c0d2c31e813ed413279c47_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> c_res = {7, c_v};
    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(c_res.degree(), a.degree());
}

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_addition_less_a) {
    std::vector<typename FieldType::value_type> a_v = {
        0x17_cppui253, 0x1a7f5666b62090e72c4090007620900000002fffffffffffe_cppui253, 0x11_cppui253,
        0x73eda753299d7d468b44719ca798c9928fb4a3fb9df55bfefffcfffeffffffff_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> a = {2, a_v};

    std::vector<typename FieldType::value_type> b_v = {
        0x37_cppui253,
        0x6C17ABF513DFFC886A7F49F970801792C825CFDD829870DC60E8DA51F53633_cppui253,
        0x73EDA753299D7D3ED0CB3E52336E8625A78AA3D929CB5BFEFFEEFFFEFFFFFFFD_cppui253,
        0x53B09574717196328488C7990499B10ABA0C038C321BF5B1C0D1C5A4E10C7330_cppui253,
        0x73EDA753299D7D483339D80809A1D80553BDA402FFFE5BFEFFFFFFFEFFFFFFEE_cppui253,
        0x73818FA734899D485AE48BE74C1F3B0838E37E245E69C38E23991724AE0AC9C4_cppui253,
        0x9626E99B5D63351DFAC330029D63300000010FFFFFFFFFFFC_cppui253,
        0x203D11DEB82BE718FE9BDD45C91A43E021C3A08591F4664D3F343A5A1EF38CC7_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> b = {7, b_v};

    a += b;

    std::vector<typename FieldType::value_type> c_v = {
        0x4e_cppui253,
        0x2984a53ad76597710bc6e589547653b47dcce72f84e0617332e6e0761851f4a9_cppui253,
        0x73eda753299d7d4078c0a4bd957794986b93a3e08bd45bfefff1fffefffffffb_cppui253,
        0x195e04ac5e7749b26f0033b1486ae23bed8b1011de0d893be16ec12aecd863c2_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffff_cppui253,
        0x4a6902185237e5dffc8fbf66155bd15f9620bcfadb4dfa8bcd291f88e7ae0b60_cppui253,
        0x7ba79334a742a436ce82a0022742a0000000dfffffffffffa_cppui253,
        0x5a8fa2a6cb26338cef1cd76f6106a8baa60293c9c1c0d2c31e813ed413279c47_cppui253};

    polynomial_dfs_view<typename FieldType::value_type> c_res = {7, c_v};
    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(c_res.degree(), a.degree());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(polynomial_dfs_view_subtraction_test_suite)

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_subtraction_equal) {

    //{1, 3, 4, 25, 6, 7, 7, 2}
    std::vector<typename FieldType::value_type> a_v = {
        0x37_cppui253,
        0x6c17abf513dffc886a7f49f970801792c825cfdd829870dc60e8da51f53633_cppui253,
        0x73eda753299d7d3ed0cb3e52336e8625a78aa3d929cb5bfeffeefffefffffffd_cppui253,
        0x53b09574717196328488c7990499b10aba0c038c321bf5b1c0d1c5a4e10c7330_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffee_cppui253,
        0x73818fa734899d485ae48be74c1f3b0838e37e245e69c38e23991724ae0ac9c4_cppui253,
        0x9626e99b5d63351dfac330029d63300000010fffffffffffc_cppui253,
        0x203d11deb82be718fe9bdd45c91a43e021c3a08591f4664d3f343a5a1ef38cc7_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> a = {7, a_v};
    //    {9, 3, 11, 14, 7, 1, 5, 8}
    std::vector<typename FieldType::value_type> b_v = {
        0x3a_cppui253,
        0x67f753af0b9db226952762685b47bb06f22600d84e5cf11425a7f17246806009_cppui253,
        0x73eda753299d7d3e43797183bd6b8154bb87a3d6b3c85bfeffedffff00000001_cppui253,
        0x4aad82754c8121ad40543e15922c725cee0fee48fc9d5b66d5fec3be9ec1d37_cppui253,
        0x6_cppui253,
        0xbf653a41dffcb283de80f4d367e56c971bba34839c56aeada640e8cb97f9ffc_cppui253,
        0x9efc066844c3656b09836002c4c3600000012000000000000_cppui253,
        0x6f42cf2bd4d56b26bf5efa79285ad71474b8a500e8108648929413c31613e2ce_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> b = {7, b_v};

    a -= b;

    //-8, 0, -7, 11, -1, 6, 2, -6
    std::vector<typename FieldType::value_type> c_v = {
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffffe_cppui253,
        0xc626b501313ab1e267cf4e9a7ca9d15f45fc8fa8f24035bb6b8f7670b74d62b_cppui253,
        0x8d51ccce760304d0ec030002760300000000fffffffffffc_cppui253,
        0x4f05bd4d1ca98417b08383b7ab76e9e4eb2b04a7a2521ffb5371d968f72055f9_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffe8_cppui253,
        0x678b3c031689d2201cfc7c9a15a0e43ec727dadc24a458a349350897f48b29c8_cppui253,
        0x73eda753299d7d47a5e80b39939ed33467baa40089fb5bfefffefffefffffffd_cppui253,
        0x24e7ea060cf3f93a7276bad4aa6144d100c89f87a9e23c03aca0269608dfa9fa_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> c_res = {7, c_v};

    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(a.degree(), c_res.degree());
}

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_subtraction_less_b) {
    // 1, 3, 4, 25, 6, 7, 7, 2
    std::vector<typename FieldType::value_type> a_v = {
        0x37_cppui253,
        0x6c17abf513dffc886a7f49f970801792c825cfdd829870dc60e8da51f53633_cppui253,
        0x73eda753299d7d3ed0cb3e52336e8625a78aa3d929cb5bfeffeefffefffffffd_cppui253,
        0x53b09574717196328488c7990499b10aba0c038c321bf5b1c0d1c5a4e10c7330_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffee_cppui253,
        0x73818fa734899d485ae48be74c1f3b0838e37e245e69c38e23991724ae0ac9c4_cppui253,
        0x9626e99b5d63351dfac330029d63300000010fffffffffffc_cppui253,
        0x203d11deb82be718fe9bdd45c91a43e021c3a08591f4664d3f343a5a1ef38cc7_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> a = {7, a_v};
    // 9, 3, 11, 14, 7
    std::vector<typename FieldType::value_type> b_v = {
        0x2c_cppui253,
        0x4e10fd71a365a0a65335b02c5b209b200de95c55c9056ed99995e2b552bf20f1_cppui253,
        0x73eda753299d7d4220b60b28f780a30b2f9ca3e7eddd5bfefff4ffff00000006_cppui253,
        0x5ed0cc0c54a3e1c092ac452c3f70cd7c6266b11a0f0dc9955edad45d54276932_cppui253,
        0xa_cppui253,
        0x25dca9e18637dcae050bc199d2c3a6d98e1647e35b3aed2566801d49ad40df14_cppui253,
        0x61283ccdf122134fa2421001b12210000000b000000000005_cppui253,
        0x151cdb46d4f99b7b7b85f91da5eea094a914f2b2ccae9269a10f2ba1abd896d3_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> b = {4, b_v};

    a -= b;
    //-8, 0, -7, 11, -1, 7, 7, 2
    std::vector<typename FieldType::value_type> c_v = {
        0xb_cppui253,
        0x2648c18d7b4bbc9e686ea725a7f1bcfcd89c6d7d147b859642cb0623ff361543_cppui253,
        0x73eda753299d7d44e34f0b31458fbb1fcbaba3f43bec5bfefff9fffefffffff8_cppui253,
        0x68cd70bb466b31ba25165a74cecabb93ab62f675230c881b61f6f1468ce509ff_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffe4_cppui253,
        0x4da4e5c5ae51c09a55d8ca4d795b942eaacd3641032ed668bd18f9db00c9eab0_cppui253,
        0x34feaccd6c4121ce58812000ec41200000005fffffffffff7_cppui253,
        0xb203697e3324b9d8315e428232ba34b78aeadd2c545d3e39e250eb8731af5f4_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> c_res = {7, c_v};

    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(a.degree(), c_res.degree());
}

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_subtraction_less_a) {

    // 1, 3, 4, 25, 6
    std::vector<typename FieldType::value_type> a_v = {
        0x27_cppui253,
        0x396e56c94dd65906159d4f74c19202bc115b4696f2872527bbf6d249d35592e2_cppui253,
        0x73eda753299d7d3c0e323e49e55f6e110b7ba3ccdbbc5bfeffe9ffff00000004_cppui253,
        0x5aedf3feb052db4e740b467d229f14d5eac1f0781703da9f46a4b599d6262364_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffff0_cppui253,
        0x3a7f5089dbc72446882aef06f827fbd0a27a5d7fbd8f36d744112db52caa6d15_cppui253,
        0xc250799be244269f448420036244200000016000000000003_cppui253,
        0x18ffb354794aa1f554a02b1736ea9ca808e3b37738e2815fb9534a6529d9dc93_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> a = {4, a_v};
    // 9, 3, 11, 14, 7, 1, 5
    std::vector<typename FieldType::value_type> b_v = {
        0x32_cppui253,
        0x19b9967b9f6b39bc04968a569c3a0628be4f3a86f3e2b7d92767e581bb4ae177_cppui253,
        0x73eda753299d7d42ae07d7f76d83a7dc1b9fa3ea63e05bfefff5ffff00000001_cppui253,
        0x4b9d19ddf7c2d17e9a88ea34c2f9262b700120efc86c79bcfea7d5dba58e1904_cppui253,
        0xe_cppui253,
        0x5a3410d78a324392ce78e75ef58c0ba7a5926999943fa425d8a41a7d44b51e8e_cppui253,
        0x5853200109c1e3029381e00189c1e0000000a000000000000_cppui253,
        0x28508d7531daabc2f8db5425be84780ed39882f5af6de242014c2a235a71e701_cppui253,
    };

    polynomial_dfs_view<typename FieldType::value_type> b = {6, b_v};

    a -= b;

    std::vector<typename FieldType::value_type> c_v = {
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffff6_cppui253,
        0x1fb4c04dae6b1f4a1106c51e2557fc93530c0c0ffea46d4e948eecc8180ab16b_cppui253,
        0x73eda753299d7d4193643e5a817d9e3a4399a3e577da5bfefff3ffff00000004_cppui253,
        0xf50da20b89009cfd9825c485fa5eeaa7ac0cf884e9760e247fcdfbe30980a60_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffe2_cppui253,
        0x5438e7057b325dfbecebdfb00c3dc82e50a597e9294deeb06b6d1336e7f54e88_cppui253,
        0x69fd599ad882439cb1024001d88240000000c000000000003_cppui253,
        0x649ccd32710d737a8efeaef98207fc9e8908d4848972fb1cb8072040cf67f593_cppui253,
    };

    polynomial_dfs_view<typename FieldType::value_type> c_res = {6, c_v};

    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(a.degree(), c_res.degree());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(polynomial_dfs_view_multiplication_test_suite)

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_multiplication_without_resize) {

    std::vector<typename FieldType::value_type> a_v = {
        0x21_cppui253,
        0x396e56c94dd65906159d4f74c19202bc115b4696f2872527bbf6d249d35592e8_cppui253,
        0x73eda753299d7d3c0e323e49e55f6e110b7ba3ccdbbc5bfeffe9fffefffffffe_cppui253,
        0x5aedf3feb052db4e740b467d229f14d5eac1f0781703da9f46a4b599d626236a_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffea_cppui253,
        0x3a7f5089dbc72446882aef06f827fbd0a27a5d7fbd8f36d744112db52caa6d1b_cppui253,
        0xc250799be244269f448420036244200000015fffffffffffd_cppui253,
        0x18ffb354794aa1f554a02b1736ea9ca808e3b37738e2815fb9534a6529d9dc99_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> a = {3, a_v};

    std::vector<typename FieldType::value_type> b_v = {
        0x17_cppui253,
        0x29188d8ee251b774835c663f5b05d39ceb04c15fa75dc9025685f79bc65cbe76_cppui253,
        0x1a7f5666b62090e72c4090007620900000002fffffffffffe_cppui253,
        0x399b168b16a330c81db144204d730936873cb088abefef89209cfb850bcbf093_cppui253,
        0x11_cppui253,
        0x4ad519c4474bc5dfd4e50b86d2de6e5cb0fae2d97ce292fca990086339a3419d_cppui253,
        0x73eda753299d7d468b44719ca798c9928fb4a3fb9df55bfefffcfffeffffffff_cppui253,
        0x3a5290c812fa4c73f080fa2997ec64da843ef3442fcc6c75df4d0479f4340f80_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> b = {2, b_v};

    a *= b;

    std::vector<typename FieldType::value_type> c_v = {
        0x2f7_cppui253,
        0xc2586e4e154f9a00f944b66e8461739a25513b55c51ca863a0c1a243371d096_cppui253,
        0x13522f003a2269a89044690056226900000023000000000048_cppui253,
        0x25831fc923df5277d906a02b53722384e786b17159eb7cb5516c7a9ea3e135e7_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffe7a_cppui253,
        0x67c8206e484883e5f76f26f4c2addc32f2b8916144fc9178c663e5dacc8e2e8f_cppui253,
        0x73eda753299d7d34e10ad7cde7382f750f54a3acdd955bfeffdcffff00000049_cppui253,
        0x4e6a878a05be2a9286699d8914dd99192ae6f17e04c2df49ae2385605c1ec93e_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> c_res = {5, c_v};

    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(c_res.degree(), a.degree());
}

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_multiplication_resize_a) {

    std::vector<typename FieldType::value_type> a_v = {
        0x21_cppui253,
        0x73eda753299d7d3c0e323e49e55f6e110b7ba3ccdbbc5bfeffe9fffefffffffe_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffea_cppui253,
        0xc250799be244269f448420036244200000015fffffffffffd_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> a = {3, a_v};

    std::vector<typename FieldType::value_type> b_v = {
        0x2f_cppui253,
        0x32e8727c0b4c2f95f8ea8699ee05fa94dda79c66917bbd26f4d3eb49c6a4019f_cppui253,
        0x73eda753299d7d4455fd3e62cf8cb64edfa8a3f1c5e95bfefff8ffff00000009_cppui253,
        0x43a84116bc8a70b038611b99d2562cf13224f12ad78417e2ba18dcf1c80c49e0_cppui253,
        0xd_cppui253,
        0x410534d71e514dbe5f56eb2c3fde4764be5807d292c49ed80b4214b5395bfe60_cppui253,
        0x3dd3c99a53a1521b6741500113a1500000007000000000008_cppui253,
        0x3045663c6d130c8bd5d122b01309411fd956b2a20438441c45d1230d37f3b61f_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> b = {5, b_v};

    a *= b;

    std::vector<typename FieldType::value_type> c_v = {
        0x60f_cppui253,
        0x6d13b656473f6fb30ca8b1bad81e6de32533efda233d7af0267de11113c42dfe_cppui253,
        0x1ba37579e72ddd31448034c66d0c9c05d6d4440bc42f8c291614691a17caee93_cppui253,
        0x1fb9403d5b7e99fd65aa34b9a80d8651511148e949d858b6cf3aa49b03f56360_cppui253,
        0x73eda753299d7cf2a2b2d70695cded866deca2858c2d5bfeff64fffeffffff4f_cppui253,
        0x270c8c72ef84d4f40592859d11b20417e6b44d040a8b017193798d7b8e32ca03_cppui253,
        0x7b8d2c73340a34c0d54fc1c4bd76e36750a43c95233e2d8de3a2088a4328cb_cppui253,
        0x6e359aacded395e1572c7d6650b9819ac85df73a6f4e6fe6f4797da6e6adf91_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffed6_cppui253,
        0x1ca7c957ddc94dd855917093ee9b65b711219a0b02e57ac9e416dcb1258452c4_cppui253,
        0x584a31d9426f9ed6c1659f7e3dbe52a8c61d5a63dd02cfd5e7a796e4e835109a_cppui253,
        0x52cd53da62d32e3299594e036edf1ebc27b1a1c2c6d4ac71dec724e0c55944ec_cppui253,
        0x559087010173d3ea7ee5d1017d73d10000009affffffffff4e_cppui253,
        0x371342853ead697d558b0c6c7aa042f9022d77750f0ac0d26485b4c03884b499_cppui253,
        0x73721a26b66974539fb88c09a3bb4a78a3390559c9a71dd174605df675bcd662_cppui253,
        0x6e7160e3c6fbf795fb196134578308430a76778e089ecbd6e0229ea7c8467781_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> c_res = {8, c_v};

    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(c_res.degree(), a.degree());
}

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_multiplication_resize_b) {

    std::vector<typename FieldType::value_type> a_v = {
        0x2f_cppui253,
        0x32e8727c0b4c2f95f8ea8699ee05fa94dda79c66917bbd26f4d3eb49c6a4019f_cppui253,
        0x73eda753299d7d4455fd3e62cf8cb64edfa8a3f1c5e95bfefff8ffff00000009_cppui253,
        0x43a84116bc8a70b038611b99d2562cf13224f12ad78417e2ba18dcf1c80c49e0_cppui253,
        0xd_cppui253,
        0x410534d71e514dbe5f56eb2c3fde4764be5807d292c49ed80b4214b5395bfe60_cppui253,
        0x3dd3c99a53a1521b6741500113a1500000007000000000008_cppui253,
        0x3045663c6d130c8bd5d122b01309411fd956b2a20438441c45d1230d37f3b61f_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> a = {5, a_v};

    std::vector<typename FieldType::value_type> b_v = {
        0x21_cppui253,
        0x73eda753299d7d3c0e323e49e55f6e110b7ba3ccdbbc5bfeffe9fffefffffffe_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffeffffffea_cppui253,
        0xc250799be244269f448420036244200000015fffffffffffd_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> b = {3, b_v};

    a *= b;

    std::vector<typename FieldType::value_type> c_v = {
        0x60f_cppui253,
        0x6d13b656473f6fb30ca8b1bad81e6de32533efda233d7af0267de11113c42dfe_cppui253,
        0x1ba37579e72ddd31448034c66d0c9c05d6d4440bc42f8c291614691a17caee93_cppui253,
        0x1fb9403d5b7e99fd65aa34b9a80d8651511148e949d858b6cf3aa49b03f56360_cppui253,
        0x73eda753299d7cf2a2b2d70695cded866deca2858c2d5bfeff64fffeffffff4f_cppui253,
        0x270c8c72ef84d4f40592859d11b20417e6b44d040a8b017193798d7b8e32ca03_cppui253,
        0x7b8d2c73340a34c0d54fc1c4bd76e36750a43c95233e2d8de3a2088a4328cb_cppui253,
        0x6e359aacded395e1572c7d6650b9819ac85df73a6f4e6fe6f4797da6e6adf91_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffed6_cppui253,
        0x1ca7c957ddc94dd855917093ee9b65b711219a0b02e57ac9e416dcb1258452c4_cppui253,
        0x584a31d9426f9ed6c1659f7e3dbe52a8c61d5a63dd02cfd5e7a796e4e835109a_cppui253,
        0x52cd53da62d32e3299594e036edf1ebc27b1a1c2c6d4ac71dec724e0c55944ec_cppui253,
        0x559087010173d3ea7ee5d1017d73d10000009affffffffff4e_cppui253,
        0x371342853ead697d558b0c6c7aa042f9022d77750f0ac0d26485b4c03884b499_cppui253,
        0x73721a26b66974539fb88c09a3bb4a78a3390559c9a71dd174605df675bcd662_cppui253,
        0x6e7160e3c6fbf795fb196134578308430a76778e089ecbd6e0229ea7c8467781_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> c_res = {8, c_v};

    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(c_res.degree(), a.degree());
}

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_multiplication_resize_both) {

    std::vector<typename FieldType::value_type> a_v = {
        0x37_cppui253,
        0x6C17ABF513DFFC886A7F49F970801792C825CFDD829870DC60E8DA51F53633_cppui253,
        0x73EDA753299D7D3ED0CB3E52336E8625A78AA3D929CB5BFEFFEEFFFEFFFFFFFD_cppui253,
        0x53B09574717196328488C7990499B10ABA0C038C321BF5B1C0D1C5A4E10C7330_cppui253,
        0x73EDA753299D7D483339D80809A1D80553BDA402FFFE5BFEFFFFFFFEFFFFFFEE_cppui253,
        0x73818FA734899D485AE48BE74C1F3B0838E37E245E69C38E23991724AE0AC9C4_cppui253,
        0x9626E99B5D63351DFAC330029D63300000010FFFFFFFFFFFC_cppui253,
        0x203D11DEB82BE718FE9BDD45C91A43E021C3A08591F4664D3F343A5A1EF38CC7_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> a = {7, a_v};

    std::vector<typename FieldType::value_type> b_v = {
        0x17_cppui253, 0x1a7f5666b62090e72c4090007620900000002fffffffffffe_cppui253, 0x11_cppui253,
        0x73eda753299d7d468b44719ca798c9928fb4a3fb9df55bfefffcfffeffffffff_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> b = {2, b_v};

    a *= b;

    std::vector<typename FieldType::value_type> c_v = {
        0x4f1_cppui253,
        0x18423f38a6347025203d1306ea24214fcbc2e3f5e167ba11285a7f2183727698_cppui253,
        0x1d4820215af065c49347299eb6dd938076c46a3db4d4f73250a46c4f2e2edaeb_cppui253,
        0x6142914f3b8f8c83dbae4715ed94fd03f84c4534f9cb1e4188a8db267f2f219b_cppui253,
        0xc250799be244269f44842003624420000001600000000003b_cppui253,
        0x4212e41081d18a85121f61c630783936244c4bde81b37f0f58d05901952e58ef_cppui253,
        0x148cedc402ae498492c4d90bffb69418494de2e305b671656a88171cc5def860_cppui253,
        0x5cdb267102977e382df877ad594da96bb37b7a71a5392773dfe1c33e2188afba_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffebe_cppui253,
        0x5ba3e41ad92b63df524868386e6ed2bf7aed342f687f8158c8c87cd5b1343b8_cppui253,
        0x56a58731cead171bd7e046caa88cbb178ac537f6a0f564ccae9f93afd1d12474_cppui253,
        0x12bf0a92313848004fcce9b9a8167b86b9595983df5f4987568536a4601e5450_cppui253,
        0x73eda753299d7d3c0e323e49e55f6e110b7ba3ccdbbc5bfeffe9ffff0000003c_cppui253,
        0x13de45c85404cdde0cd67ace68479511641ba792a6772ac8f4fca00e8c4beb9a_cppui253,
        0x5f60b98f26ef342b6887669ab422cd5a5ca3c2eea47bea999633e8e23a2106ff_cppui253,
        0x16fe8c53e3dba6560be2697c242189564a3e2834817d28c13e3c2af4ff29d935_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> c_res = {9, c_v};

    BOOST_CHECK_EQUAL(c_res.size(), a.size());
    for (std::size_t i = 0; i < c_res.size(); i++) {
        BOOST_CHECK_EQUAL(c_res[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(c_res.degree(), a.degree());
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(polynomial_dfs_view_division_test_suite)

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_division) {
    // {5, 0, 0, 13, 0, 1};
    std::vector<typename FieldType::value_type> a_v = {
        0x13_cppui253,
        0x515afe1189d5ef4dbc50a127ce5e634034a28d0005e1fafd70aeef634654d2e0_cppui253,
        0x73eda753299d7d4193643e5a817d9e3a4399a3e577da5bfefff3ffff00000006_cppui253,
        0x519843b006c2b71461725846c0416002ece7f29b47582f326bdcdd22024de904_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffff8_cppui253,
        0x2292a9419fc78dfa76e936e03b4374c51f1b1702fa1c61018f51109bb9ab2d2b_cppui253,
        0x69fd599ad882439cb1024001d88240000000c000000000005_cppui253,
        0x225563a322dac633d1c77fc14960780266d5b167b8a62ccc942322dcfdb21707_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> a = {5, a_v};
    //{13, 0, 1};
    std::vector<typename FieldType::value_type> b_v = {
        0xe_cppui253, 0x8d51ccce760304d0ec03000276030000000100000000000d_cppui253,
        0xc_cppui253, 0x73eda753299d7d47a5e80b39939ed33467baa40089fb5bfefffeffff0000000e_cppui253,
        0xe_cppui253, 0x8d51ccce760304d0ec03000276030000000100000000000d_cppui253,
        0xc_cppui253, 0x73eda753299d7d47a5e80b39939ed33467baa40089fb5bfefffeffff0000000e_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> b = {2, b_v};

    a /= b;

    std::vector<typename FieldType::value_type> q_v = {
        0x1_cppui253,
        0x1333b22e5ce11044babc5affca86bf658e74903694b04fd86037fe81ae99502e_cppui253,
        0x73eda753299d7d47a5e80b39939ed33467baa40089fb5bfefffeffff00000001_cppui253,
        0x345766f603fa66e78c0625cd70d77ce2b38b21c28713b7007228fd3397743f7a_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfeffffffff00000000_cppui253,
        0x60b9f524ccbc6d03787d7d083f1b189fc54913cc6b4e0c269fc8017d5166afd3_cppui253,
        0x8d51ccce760304d0ec030002760300000001000000000000_cppui253,
        0x3f96405d25a31660a733b23a98ca5b22a032824078eaa4fe8dd702cb688bc087_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> q_ans = {3, q_v};    // {0, 0, 0, 1};

    BOOST_CHECK_EQUAL(q_ans.size(), a.size());
    for (std::size_t i = 0; i < a.size(); i++) {
        BOOST_CHECK_EQUAL(q_ans[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(q_ans.degree(), a.degree());
}

BOOST_AUTO_TEST_CASE(polynomial_dfs_view_modulus) {
    // {5, 0, 0, 13, 0, 1};
    std::vector<typename FieldType::value_type> a_v = {
        0x13_cppui253,
        0x515afe1189d5ef4dbc50a127ce5e634034a28d0005e1fafd70aeef634654d2e0_cppui253,
        0x73eda753299d7d4193643e5a817d9e3a4399a3e577da5bfefff3ffff00000006_cppui253,
        0x519843b006c2b71461725846c0416002ece7f29b47582f326bdcdd22024de904_cppui253,
        0x73eda753299d7d483339d80809a1d80553bda402fffe5bfefffffffefffffff8_cppui253,
        0x2292a9419fc78dfa76e936e03b4374c51f1b1702fa1c61018f51109bb9ab2d2b_cppui253,
        0x69fd599ad882439cb1024001d88240000000c000000000005_cppui253,
        0x225563a322dac633d1c77fc14960780266d5b167b8a62ccc942322dcfdb21707_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> a = {5, a_v};
    //{13, 0, 1};
    std::vector<typename FieldType::value_type> b_v = {
        0xe_cppui253, 0x8d51ccce760304d0ec03000276030000000100000000000d_cppui253,
        0xc_cppui253, 0x73eda753299d7d47a5e80b39939ed33467baa40089fb5bfefffeffff0000000e_cppui253,
        0xe_cppui253, 0x8d51ccce760304d0ec03000276030000000100000000000d_cppui253,
        0xc_cppui253, 0x73eda753299d7d47a5e80b39939ed33467baa40089fb5bfefffeffff0000000e_cppui253,
    };
    polynomial_dfs_view<typename FieldType::value_type> b = {2, b_v};

    a %= b;

    std::vector<typename FieldType::value_type> r_v = {0x5_cppui253, 0x5_cppui253, 0x5_cppui253, 0x5_cppui253,
                                                       0x5_cppui253, 0x5_cppui253, 0x5_cppui253, 0x5_cppui253};
    polynomial_dfs_view<typename FieldType::value_type> r_ans = {0, r_v};    //{5};

    BOOST_CHECK_EQUAL(r_ans.size(), a.size());
    for (std::size_t i = 0; i < a.size(); i++) {
        BOOST_CHECK_EQUAL(r_ans[i].data, a[i].data);
    }
    BOOST_CHECK_EQUAL(r_ans.degree(), a.degree());
}

BOOST_AUTO_TEST_SUITE_END()