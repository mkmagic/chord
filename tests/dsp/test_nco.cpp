#include <chord/dsp/nco.hpp>

#include <gtest/gtest.h>

using chord::dsp::nco_generate;
using chord::dsp::nco_generate_complex;
using chord::dsp::NcoState;
using namespace kfr;

TEST(NcoTest, PhaseGeneration) {
    // f = 1000, fs = 8000. Step = 2*pi * (1/8) = pi/4
    const float pi = kfr::c_pi<float, 1>;
    const float step = pi / 4.0f;
    univector<float> out(4);

    NcoState state;
    nco_generate(out.ref(), state, 1000.0f, 8000.0f);

    EXPECT_FLOAT_EQ(out[0], 0.0f);
    EXPECT_FLOAT_EQ(out[1], step);
    EXPECT_FLOAT_EQ(out[2], step * 2.0f);
    EXPECT_FLOAT_EQ(out[3], step * 3.0f);

    // State should wrap to next sample (step * 4 = pi => wraps to -pi)
    EXPECT_FLOAT_EQ(state.phase, -pi);
}

TEST(NcoTest, ComplexGenerationTracking) {
    // Same frequency, we ensure the array boundary splits are lossless
    const size_t total_size = 100;
    const float freq = 123.456f;
    const float fs = 1000.0f;

    univector<complex<float>> out_sync(total_size);
    NcoState sync_state;
    nco_generate_complex(out_sync.ref(), sync_state, freq, fs);

    univector<complex<float>> out_split(total_size);
    NcoState split_state;

    // Split generation exactly like streaming buffers
    nco_generate_complex(out_split.slice(0, 31), split_state, freq, fs);
    nco_generate_complex(out_split.slice(31, 14), split_state, freq, fs);
    nco_generate_complex(out_split.slice(45, 55), split_state, freq, fs);

    for (size_t i = 0; i < total_size; ++i) {
        // High frequency bounds are checked with near epsilons due to internal trig
        EXPECT_NEAR(out_sync[i].real(), out_split[i].real(), 1e-5f);
        EXPECT_NEAR(out_sync[i].imag(), out_split[i].imag(), 1e-5f);
    }
}
