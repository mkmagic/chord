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
    kfr::complex<float> sum{0.0f, 0.0f};
    chord::Status status = chord::math::sum_conjugate_product(input, sum, 1);

    EXPECT_EQ(status, chord::Status::OK);

    EXPECT_NEAR(sum.real(), 0.0f, 1e-6);
    EXPECT_NEAR(sum.imag(), 3.0f, 1e-6);
}

TEST(CorrelationTest, SumConjugateProductLagTooLarge) {
    kfr::univector<kfr::complex<float>> input = {{1.0f, 0.0f}, {0.0f, 1.0f}};

    kfr::complex<float> sum_equal{0.0f, 0.0f};
    kfr::complex<float> sum_greater{0.0f, 0.0f};
    chord::Status status_equal = chord::math::sum_conjugate_product(input, sum_equal, input.size());
    chord::Status status_greater =
        chord::math::sum_conjugate_product(input, sum_greater, input.size() + 1);

    EXPECT_EQ(status_equal, chord::Status::INPUT_TOO_SMALL);
    EXPECT_EQ(status_greater, chord::Status::INPUT_TOO_SMALL);

    EXPECT_NEAR(sum_equal.real(), 0.0f, 1e-6);
    EXPECT_NEAR(sum_equal.imag(), 0.0f, 1e-6);
    EXPECT_NEAR(sum_greater.real(), 0.0f, 1e-6);
    EXPECT_NEAR(sum_greater.imag(), 0.0f, 1e-6);
}
