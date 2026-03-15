#include <chord/math/correlation.hpp>

#include <gtest/gtest.h>

TEST(CorrelationTest, SumConjugateProduct) {
    kfr::univector<kfr::complex<float>> input = {{1.0f, 0.0f},
                                                 {0.0f, 1.0f},
                                                 {-1.0f, 0.0f},
                                                 {0.0f, -1.0f}};

    // lag 1
    // (0,1)*(1,0) + (-1,0)*(0,-1) + (0,-1)*(-1,0)
    // = i*1 + -1*(-i) + (-i)*(-1)
    // = i + i + i = 3i
    kfr::complex<float> sum = chord::math::sum_conjugate_product(input, 1);

    EXPECT_NEAR(sum.real(), 0.0f, 1e-6);
    EXPECT_NEAR(sum.imag(), 3.0f, 1e-6);
}

TEST(CorrelationTest, SumConjugateProductLagTooLarge) {
    kfr::univector<kfr::complex<float>> input = {{1.0f, 0.0f}, {0.0f, 1.0f}};

    kfr::complex<float> sum_equal = chord::math::sum_conjugate_product(input, input.size());
    kfr::complex<float> sum_greater = chord::math::sum_conjugate_product(input, input.size() + 1);

    EXPECT_NEAR(sum_equal.real(), 0.0f, 1e-6);
    EXPECT_NEAR(sum_equal.imag(), 0.0f, 1e-6);
    EXPECT_NEAR(sum_greater.real(), 0.0f, 1e-6);
    EXPECT_NEAR(sum_greater.imag(), 0.0f, 1e-6);
}
