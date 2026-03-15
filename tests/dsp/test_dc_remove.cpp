#include <chord/dsp/dc_remove.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>

using chord::dsp::dc_remove;
using chord::dsp::DcRemoveState;
using namespace kfr;

TEST(DcRemoveTest, DcConvergesToZero) {
    const float cutoff_hz = 5.0f;
    const float sample_rate = 1000.0f;

    univector<float> in(256, 1.0f);
    univector<float> out(in.size());

    DcRemoveState state;
    chord::Status status = dc_remove(in, out, state, cutoff_hz, sample_rate);

    EXPECT_EQ(status, chord::Status::OK);
    EXPECT_NEAR(out.back(), 0.0f, 1e-2f);
}

TEST(DcRemoveTest, StreamingContinuity) {
    const float cutoff_hz = 10.0f;
    const float sample_rate = 1000.0f;

    univector<float> in(128);
    for (size_t i = 0; i < in.size(); ++i) {
        in[i] = std::sin(2.0f * kfr::c_pi<float> * 50.0f * static_cast<float>(i) / sample_rate) + 0.5f;
    }

    univector<float> out_full(in.size());
    univector<float> out_chunked(in.size());

    DcRemoveState state_full;
    DcRemoveState state_chunked;

    chord::Status status_full = dc_remove(in, out_full, state_full, cutoff_hz, sample_rate);
    EXPECT_EQ(status_full, chord::Status::OK);

    size_t offset = 0;
    while (offset < in.size()) {
        const size_t chunk = std::min<size_t>(11, in.size() - offset);
        auto in_slice = in.slice(offset, chunk);
        auto out_slice = out_chunked.slice(offset, chunk);
        chord::Status status_chunk = dc_remove(in_slice, out_slice, state_chunked, cutoff_hz, sample_rate);
        EXPECT_EQ(status_chunk, chord::Status::OK);
        offset += chunk;
    }

    for (size_t i = 0; i < in.size(); ++i) {
        EXPECT_NEAR(out_chunked[i], out_full[i], 1e-5f);
    }
}

TEST(DcRemoveTest, InvalidCutoff) {
    univector<float> in(4, 1.0f);
    univector<float> out(in.size());
    DcRemoveState state;

    chord::Status status = dc_remove(in, out, state, 0.0f, 1000.0f);

    EXPECT_EQ(status, chord::Status::INVALID_PARAM);
}

TEST(DcRemoveTest, OutputTooSmall) {
    univector<float> in(4, 1.0f);
    univector<float> out(1);
    DcRemoveState state;

    chord::Status status = dc_remove(in, out, state, 5.0f, 1000.0f);

    EXPECT_EQ(status, chord::Status::OUTPUT_TOO_SMALL);
}
