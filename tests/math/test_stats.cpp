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
    exponential_moving_average(in, out, state, 1.0f);
    EXPECT_FLOAT_EQ(out[0], 1.0f);
    EXPECT_FLOAT_EQ(state.average, 1.0f);

    // Reset and test slow tracking
    state = ExponentialMovingAverageState();
    // Step response: 0 to 1 with alpha 0.1
    exponential_moving_average(in, out, state, 0.1f);

    // First element should be initialized exactly to 1.0f due to initialization logic
    EXPECT_FLOAT_EQ(out[0], 1.0f);
    EXPECT_FLOAT_EQ(out[50], 1.0f);
}

TEST(StatsTest, EMAHandlesEmptyInput) {
    univector<float> in;
    univector<float> out;

    ExponentialMovingAverageState state;
    state.average = 2.0f;
    state.initialized = true;

    exponential_moving_average(in, out, state, 0.3f);
    EXPECT_FLOAT_EQ(state.average, 2.0f);
    EXPECT_TRUE(state.initialized);
}

TEST(StatsTest, EMAInitializesSingleSample) {
    univector<float> in = {2.0f};
    univector<float> out(1);

    ExponentialMovingAverageState state;

    exponential_moving_average(in, out, state, 0.3f);
    EXPECT_FLOAT_EQ(out[0], 2.0f);
    EXPECT_FLOAT_EQ(state.average, 2.0f);
    EXPECT_TRUE(state.initialized);
}

TEST(StatsTest, EMACopiesForAlphaOne) {
    univector<float> in = {1.0f, 2.0f, 3.0f};
    univector<float> out(in.size());

    ExponentialMovingAverageState state;

    exponential_moving_average(in, out, state, 1.0f);
    EXPECT_FLOAT_EQ(out[0], 1.0f);
    EXPECT_FLOAT_EQ(out[1], 2.0f);
    EXPECT_FLOAT_EQ(out[2], 3.0f);
    EXPECT_FLOAT_EQ(state.average, 3.0f);
    EXPECT_TRUE(state.initialized);
}

TEST(StatsTest, EMAStepResponse) {
    univector<float> in = {1.0f, 1.0f, 1.0f, 1.0f};
    univector<float> out(4);

    ExponentialMovingAverageState state;
    // Set initial state to 0 so we see the step response immediately
    state.average = 0.0f;
    state.initialized = true;

    exponential_moving_average(in, out, state, 0.5f);

    // out[0] = 0.5*1.0 + 0.5*0 = 0.5
    EXPECT_FLOAT_EQ(out[0], 0.5f);
    // out[1] = 0.5*1.0 + 0.5*0.5 = 0.75
    EXPECT_FLOAT_EQ(out[1], 0.75f);
    // out[2] = 0.5*1.0 + 0.5*0.75 = 0.875
    EXPECT_FLOAT_EQ(out[2], 0.875f);
}

TEST(StatsTest, EMAStreamingContinuity) {
    univector<float> in(64);
    for (size_t i = 0; i < in.size(); ++i) {
        in[i] = static_cast<float>(i) * 0.1f;
    }

    univector<float> out_full(in.size());
    univector<float> out_chunked(in.size());

    ExponentialMovingAverageState state_full;
    ExponentialMovingAverageState state_chunked;

    const float alpha = 0.25f;
    exponential_moving_average(in, out_full, state_full, alpha);

    size_t offset = 0;
    while (offset < in.size()) {
        const size_t chunk = std::min<size_t>(7, in.size() - offset);
        auto in_slice = in.slice(offset, chunk);
        auto out_slice = out_chunked.slice(offset, chunk);
        exponential_moving_average(in_slice, out_slice, state_chunked, alpha);
        offset += chunk;
    }

    for (size_t i = 0; i < in.size(); ++i) {
        EXPECT_FLOAT_EQ(out_chunked[i], out_full[i]);
    }
}

TEST(StatsTest, EMVHandlesEmptyInput) {
    univector<float> in;
    univector<float> out_var;

    ExponentialMovingVarianceState state;
    state.average = 1.0f;
    state.variance = 2.0f;
    state.initialized = true;

    exponential_moving_variance(in, out_var, state, 0.2f);
    EXPECT_FLOAT_EQ(state.average, 1.0f);
    EXPECT_FLOAT_EQ(state.variance, 2.0f);
    EXPECT_TRUE(state.initialized);
}

TEST(StatsTest, EMVInitializesAndTracks) {
    univector<float> in = {2.0f, 2.0f, 2.0f};
    univector<float> out_var(in.size());

    ExponentialMovingVarianceState state;

    exponential_moving_variance(in, out_var, state, 0.3f);
    EXPECT_TRUE(state.initialized);
    EXPECT_FLOAT_EQ(out_var[0], 0.0f);
}

TEST(StatsTest, EMVCopiesZeroVarianceForAlphaOne) {
    univector<float> in = {1.0f, -1.0f, 1.0f};
    univector<float> out_var(in.size());

    ExponentialMovingVarianceState state;

    exponential_moving_variance(in, out_var, state, 1.0f);
    EXPECT_FLOAT_EQ(out_var[0], 0.0f);
    EXPECT_FLOAT_EQ(out_var[1], 0.0f);
    EXPECT_FLOAT_EQ(out_var[2], 0.0f);
    EXPECT_FLOAT_EQ(state.average, 1.0f);
    EXPECT_FLOAT_EQ(state.variance, 0.0f);
    EXPECT_TRUE(state.initialized);
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
    exponential_moving_variance(in, out_var, state, 0.1f);

    // The variance should be hovering near 1.0
    EXPECT_GT(out_var[5], 0.5f);
    EXPECT_LT(out_var[5], 1.5f);
}
