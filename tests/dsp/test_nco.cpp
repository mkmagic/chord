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
    chord::Status status = nco_generate(out.ref(), state, 1000.0f, 8000.0f);

    EXPECT_EQ(status, chord::Status::OK);

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
    chord::Status sync_status = nco_generate_complex(out_sync.ref(), sync_state, freq, fs);

    EXPECT_EQ(sync_status, chord::Status::OK);

    univector<complex<float>> out_split(total_size);
    NcoState split_state;

    // Split generation exactly like streaming buffers
    chord::Status status1 = nco_generate_complex(out_split.slice(0, 31), split_state, freq, fs);
    chord::Status status2 = nco_generate_complex(out_split.slice(31, 14), split_state, freq, fs);
    chord::Status status3 = nco_generate_complex(out_split.slice(45, 55), split_state, freq, fs);

    EXPECT_EQ(status1, chord::Status::OK);
    EXPECT_EQ(status2, chord::Status::OK);
    EXPECT_EQ(status3, chord::Status::OK);

    for (size_t i = 0; i < total_size; ++i) {
        // High frequency bounds are checked with near epsilons due to internal trig
        EXPECT_NEAR(out_sync[i].real(), out_split[i].real(), 1e-5f);
        EXPECT_NEAR(out_sync[i].imag(), out_split[i].imag(), 1e-5f);
    }
}

TEST(NcoTest, ComplexGain) {
    // gain=2.0 should double the magnitude of a unit-amplitude sinusoid.
    const size_t N = 16;
    univector<complex<float>> out_unit(N), out_gained(N);
    NcoState state_unit, state_gained;
    chord::Status status_unit = nco_generate_complex(out_unit.ref(), state_unit, 100.0f, 1000.0f);
    chord::Status status_gained =
        nco_generate_complex(out_gained.ref(), state_gained, 100.0f, 1000.0f, 2.0f);

    EXPECT_EQ(status_unit, chord::Status::OK);
    EXPECT_EQ(status_gained, chord::Status::OK);

    for (size_t i = 0; i < N; ++i) {
        EXPECT_NEAR(out_gained[i].real(), out_unit[i].real() * 2.0f, 1e-6f);
        EXPECT_NEAR(out_gained[i].imag(), out_unit[i].imag() * 2.0f, 1e-6f);
    }
}
