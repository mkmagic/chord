#include <chord/math/phase.hpp>

#include <gtest/gtest.h>

using chord::math::instantaneous_frequency;
using chord::math::PhaseUnwrapState;
using chord::math::unwrap_phase;
using namespace kfr;

TEST(PhaseTest, UnwrapsPositiveJumps) {
    univector<float> in = {3.0f, -3.0f, -2.5f};
    univector<float> out(in.size());

    PhaseUnwrapState state;
    unwrap_phase(in.ref(), out.ref(), state);

    const float two_pi = c_pi<float, 2>;
    EXPECT_FLOAT_EQ(out[0], 3.0f);
    // The jump from 3.0 to -3.0 is -6.0. Since this is < -PI, it adds 2PI (~6.28)
    // -3.0 + 2PI = 3.2831853
    EXPECT_NEAR(out[1], -3.0f + two_pi, 1e-5);
    EXPECT_NEAR(out[2], -2.5f + two_pi, 1e-5);
}

TEST(PhaseTest, StreamingUnwrapping) {
    univector<float> b1 = {3.0f};
    univector<float> b2 = {-3.0f};

    univector<float> out1(1);
    univector<float> out2(1);

    PhaseUnwrapState state;
    unwrap_phase(b1.ref(), out1.ref(), state);
    unwrap_phase(b2.ref(), out2.ref(), state);

    const float two_pi = c_pi<float, 2>;
    EXPECT_FLOAT_EQ(out1[0], 3.0f);
    EXPECT_NEAR(out2[0], -3.0f + two_pi, 1e-5);
}

TEST(FreqTest, InstFreqCalculation) {
    univector<float> in = {1.0f, 2.0f, 3.5f};
    univector<float> out(in.size());

    float prev_phase = 0.0f;
    instantaneous_frequency(in.ref(), out.ref(), prev_phase, 2.0f);

    EXPECT_FLOAT_EQ(out[0], 2.0f);  // (1.0 - 0.0) * 2.0
    EXPECT_FLOAT_EQ(out[1], 2.0f);  // (2.0 - 1.0) * 2.0
    EXPECT_FLOAT_EQ(out[2], 3.0f);  // (3.5 - 2.0) * 2.0
}
