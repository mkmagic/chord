#include <chord/math/stats.hpp>

#include <gtest/gtest.h>

using chord::math::exponential_moving_average;
using chord::math::exponential_moving_variance;
using chord::math::ExponentialMovingAverageState;
using chord::math::ExponentialMovingVarianceState;
using namespace kfr;

TEST(StatsTest, EMATracking) {
    univector<float> in(100, 1.0f);
    univector<float> out(100);

    ExponentialMovingAverageState state;

    // Test alpha = 1.0 (instant jump)
    exponential_moving_average(in.ref(), out.ref(), state, 1.0f);
    EXPECT_FLOAT_EQ(out[0], 1.0f);
    EXPECT_FLOAT_EQ(state.average, 1.0f);

    // Reset and test slow tracking
    state = ExponentialMovingAverageState();
    // Step response: 0 to 1 with alpha 0.1
    exponential_moving_average(in.ref(), out.ref(), state, 0.1f);

    // First element should be initialized exactly to 1.0f due to initialization logic
    EXPECT_FLOAT_EQ(out[0], 1.0f);
    EXPECT_FLOAT_EQ(out[50], 1.0f);
}

TEST(StatsTest, EMAStepResponse) {
    univector<float> in = {1.0f, 1.0f, 1.0f, 1.0f};
    univector<float> out(4);

    ExponentialMovingAverageState state;
    // Set initial state to 0 so we see the step response immediately
    state.average = 0.0f;
    state.initialized = true;

    exponential_moving_average(in.ref(), out.ref(), state, 0.5f);

    // out[0] = 0.5*1.0 + 0.5*0 = 0.5
    EXPECT_FLOAT_EQ(out[0], 0.5f);
    // out[1] = 0.5*1.0 + 0.5*0.5 = 0.75
    EXPECT_FLOAT_EQ(out[1], 0.75f);
    // out[2] = 0.5*1.0 + 0.5*0.75 = 0.875
    EXPECT_FLOAT_EQ(out[2], 0.875f);
}

TEST(StatsTest, EMVVariance) {
    // Alternating signal: var = 1, mean = 0
    univector<float> in = {1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f};
    univector<float> out_var(in.size());

    ExponentialMovingVarianceState state;
    state.average = 0.0f;
    state.variance = 1.0f;
    state.initialized = true;

    // Small alpha means steady-state tracks closely
    exponential_moving_variance(in.ref(), out_var.ref(), state, 0.1f);

    // The variance should be hovering near 1.0
    EXPECT_GT(out_var[5], 0.5f);
    EXPECT_LT(out_var[5], 1.5f);
}
