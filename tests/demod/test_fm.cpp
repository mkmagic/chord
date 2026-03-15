#include <chord/demod/fm.hpp>

#include <gtest/gtest.h>

using chord::demod::fm_demodulate;
using chord::demod::FmDemodulatorState;
using namespace kfr;

TEST(FmDemodTest, SimplePhaseRamp) {
    // A constant phase ramp (frequency offset) should demodulate
    // to a constant DC value.
    // phase = 0, pi/4, 2pi/4, 3pi/4...
    // diff = pi/4
    const float diff = kfr::c_pi<float, 1> / 4.0f;

    univector<complex<float>> in = {std::polar(1.0f, 0.0f),
                                    std::polar(1.0f, diff),
                                    std::polar(1.0f, diff * 2),
                                    std::polar(1.0f, diff * 3)};
    univector<float> out(4);

    FmDemodulatorState state;
    // Initial state matching the start of the ramp
    state.previous_sample = std::polar(1.0f, -diff);

    chord::Status status = fm_demodulate(in.ref(), out.ref(), state, 1.0f);

    EXPECT_EQ(status, chord::Status::OK);

    EXPECT_FLOAT_EQ(out[0], diff);
    EXPECT_FLOAT_EQ(out[1], diff);
    EXPECT_FLOAT_EQ(out[2], diff);
    EXPECT_FLOAT_EQ(out[3], diff);
}

TEST(FmDemodTest, EmptyInput) {
    univector<complex<float>> in;
    univector<float> out(1);

    FmDemodulatorState state;
    chord::Status status = fm_demodulate(in.ref(), out.ref(), state, 1.0f);

    EXPECT_EQ(status, chord::Status::INPUT_TOO_SMALL);
}

TEST(FmDemodTest, OutputTooSmall) {
    univector<complex<float>> in = {std::polar(1.0f, 0.0f), std::polar(1.0f, 0.1f)};
    univector<float> out(1);

    FmDemodulatorState state;
    chord::Status status = fm_demodulate(in.ref(), out.ref(), state, 1.0f);

    EXPECT_EQ(status, chord::Status::OUTPUT_TOO_SMALL);
}

TEST(FmDemodTest, BlockBoundaryContinuity) {
    // Generate an FM signal
    const size_t total_size = 100;
    univector<float> audio(total_size);
    for (size_t i = 0; i < total_size; ++i) {
        audio[i] = std::sin(2.0f * kfr::c_pi<float, 1> * (i / 100.0f));
    }

    // Naively modulate it: phase integral
    univector<complex<float>> modulated(total_size);
    float current_phase = 0.0f;
    for (size_t i = 0; i < total_size; ++i) {
        current_phase += audio[i];
        modulated[i] = std::polar(1.0f, current_phase);
    }

    // Benchmark: Demodulate entire block synchronously
    univector<float> out_sync(total_size);
    FmDemodulatorState sync_state;
    sync_state.previous_sample = std::polar(1.0f, 0.0f);
    chord::Status sync_status = fm_demodulate(modulated.ref(), out_sync.ref(), sync_state, 1.0f);

    EXPECT_EQ(sync_status, chord::Status::OK);

    // Test: Demodulate across arbitrary splits
    univector<float> out_split(total_size);
    FmDemodulatorState split_state;
    split_state.previous_sample = std::polar(1.0f, 0.0f);

    // Split into 3 arbitrary chunks: 14, 53, and 33
    chord::Status status1 =
        fm_demodulate(modulated.slice(0, 14), out_split.slice(0, 14), split_state, 1.0f);
    chord::Status status2 =
        fm_demodulate(modulated.slice(14, 53), out_split.slice(14, 53), split_state, 1.0f);
    chord::Status status3 =
        fm_demodulate(modulated.slice(67, 33), out_split.slice(67, 33), split_state, 1.0f);

    EXPECT_EQ(status1, chord::Status::OK);
    EXPECT_EQ(status2, chord::Status::OK);
    EXPECT_EQ(status3, chord::Status::OK);

    // The chunks must produce the exact identical floating point output
    // as the synchronously processed buffer.
    for (size_t i = 0; i < total_size; ++i) {
        EXPECT_FLOAT_EQ(out_split[i], out_sync[i]);
    }
}
