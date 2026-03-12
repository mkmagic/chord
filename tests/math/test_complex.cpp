#include <chord/math/complex.hpp>

#include <gtest/gtest.h>

using chord::math::conjugate_multiply;
using chord::math::magnitude_squared;
using namespace kfr;

TEST(ComplexTest, ConjugateMultiply) {
    univector<complex<float>> a = {complex<float>(1.0f, 2.0f), complex<float>(-1.0f, 0.0f)};
    univector<complex<float>> b = {complex<float>(1.0f, 1.0f), complex<float>(0.0f, 1.0f)};
    univector<complex<float>> out(2);

    conjugate_multiply(a.ref(), b.ref(), out.ref());

    // (1 + 2j) * (1 - 1j) = 1 - 1j + 2j - 2(j^2) = 1 + 1j + 2 = 3 + 1j
    EXPECT_FLOAT_EQ(out[0].real(), 3.0f);
    EXPECT_FLOAT_EQ(out[0].imag(), 1.0f);

    // (-1 + 0j) * (0 - 1j) = 0 + 1j = 0 + 1j
    EXPECT_FLOAT_EQ(out[1].real(), 0.0f);
    EXPECT_FLOAT_EQ(out[1].imag(), 1.0f);
}

TEST(ComplexTest, MagnitudeSquared) {
    univector<complex<float>> in = {complex<float>(3.0f, 4.0f), complex<float>(1.0f, -1.0f)};
    univector<float> out(2);

    magnitude_squared(in.ref(), out.ref());

    // 3^2 + 4^2 = 9 + 16 = 25
    EXPECT_FLOAT_EQ(out[0], 25.0f);

    // 1^2 + (-1)^2 = 1 + 1 = 2
    EXPECT_FLOAT_EQ(out[1], 2.0f);
}
