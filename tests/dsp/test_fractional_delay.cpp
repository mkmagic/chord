#include <chord/dsp/fractional_delay.hpp>
#include <gtest/gtest.h>

using namespace chord::dsp;
using namespace kfr;

TEST(FractionalDelayTest, IntegerDelay) {
    auto state = make_fractional_delay_state(10);
    univector<float> in = {1.0f, 2.0f, 3.0f, 4.0f};
    univector<float> out(4);
    univector<float> delays = {2.0f, 2.0f, 2.0f, 2.0f};

    fractional_delay(in, out, delays, state);

    // Initial output should be zero (or previous state)
    // with delay 2.0:
    // i=0: in[0]=1.0 written. out[0] = buffer[idx-2] = 0.0
    // i=1: in[1]=2.0 written. out[1] = buffer[idx-2] = 0.0
    // i=2: in[2]=3.0 written. out[2] = buffer[idx-2] = 1.0 (from i=0)
    // i=3: in[3]=4.0 written. out[3] = buffer[idx-2] = 2.0 (from i=1)
    EXPECT_FLOAT_EQ(out[0], 0.0f);
    EXPECT_FLOAT_EQ(out[1], 0.0f);
    EXPECT_FLOAT_EQ(out[2], 1.0f);
    EXPECT_FLOAT_EQ(out[3], 2.0f);
}

TEST(FractionalDelayTest, LinearInterpolation) {
    auto state = make_fractional_delay_state(10);
    univector<float> in = {10.0f, 20.0f, 30.0f};
    univector<float> out(3);
    univector<float> delays = {1.5f, 1.5f, 1.5f};

    fractional_delay(in, out, delays, state);

    // i=0: in[0]=10.0. out[0] = interp(0.5, buffer[k], buffer[k-1]) = interp(0.5, 0, 0) = 0.0
    // i=1: in[1]=20.0. out[1] = interp(0.5, 10.0, 0.0) = 5.0
    // i=2: in[2]=30.0. out[2] = interp(0.5, 20.0, 10.0) = 15.0
    EXPECT_FLOAT_EQ(out[0], 0.0f);
    EXPECT_FLOAT_EQ(out[1], 5.0f);
    EXPECT_FLOAT_EQ(out[2], 15.0f);
}

TEST(FractionalDelayTest, CubicInterpolation) {
    auto state = make_fractional_delay_state(10);
    // Use a ramp to test cubic
    univector<float> in = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    univector<float> out(6);
    univector<float> delays = {2.5f, 2.5f, 2.5f, 2.5f, 2.5f, 2.5f};

    fractional_delay(in, out, delays, state, InterpolationType::Cubic);

    // For a linear ramp, cubic interpolation should still yield linear results
    // i=0..2 will have some zeros
    // i=3: in={0,1,2,3}. delay=2.5. out[3] is between in[3-2]=in[1] and in[3-3]=in[0]
    // Wait, k = write_idx - 2. For i=3, write_idx=3. k=1.
    // Points for k=1: y0=buffer[2]=2.0, y1=buffer[1]=1.0, y2=buffer[0]=0.0, y3=buffer[-1]=0.0
    // frac=0.5. interp(0.5, 2.0, 1.0, 0.0, 0.0)
    
    // Let's check i=5: in={0,1,2,3,4,5}. delay=2.5. 
    // write_idx=5. k=5-2=3.
    // y0=buffer[4]=4.0, y1=buffer[3]=3.0, y2=buffer[2]=2.0, y3=buffer[1]=1.0
    // frac=0.5. interp(0.5, 4.0, 3.0, 2.0, 1.0) = 2.5
    EXPECT_NEAR(out[5], 2.5f, 1e-6f);
}

TEST(FractionalDelayTest, StreamingContinuity) {
    const size_t max_delay = 5;
    const size_t N = 20;
    univector<float> in(N);
    for(size_t i=0; i<N; ++i) in[i] = static_cast<float>(i);
    univector<float> delays(N, 1.25f);

    // Single block
    auto state1 = make_fractional_delay_state(max_delay);
    univector<float> out1(N);
    fractional_delay(in, out1, delays, state1, InterpolationType::Cubic);

    // Multiple blocks
    auto state2 = make_fractional_delay_state(max_delay);
    univector<float> out2(N);
    fractional_delay(in.slice(0, 7), out2.slice(0, 7), delays.slice(0, 7), state2, InterpolationType::Cubic);
    fractional_delay(in.slice(7, 5), out2.slice(7, 5), delays.slice(7, 5), state2, InterpolationType::Cubic);
    fractional_delay(in.slice(12, 8), out2.slice(12, 8), delays.slice(12, 8), state2, InterpolationType::Cubic);

    for(size_t i=0; i<N; ++i) {
        EXPECT_FLOAT_EQ(out1[i], out2[i]);
    }
}
