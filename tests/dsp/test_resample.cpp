#include <chord/dsp/resample.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <vector>

using chord::dsp::decimate;
using chord::dsp::decimate_output_size;
using chord::dsp::ResamplerState;
using chord::dsp::upsample;
using chord::dsp::upsample_output_size;
using namespace kfr;

TEST(DecimateTest, MatchesKfrConverter) {
    const float sample_rate = 1000.0f;
    const float tone = 50.0f;
    const size_t factor = 2;

    univector<float> in(256);
    for (size_t i = 0; i < in.size(); ++i) {
        in[i] = std::sin(2.0f * kfr::c_pi<float> * tone * static_cast<float>(i) / sample_rate);
    }

    const size_t expected = decimate_output_size(in.size(), factor);
    univector<float> out(expected);
    univector<float> ref(expected);

    ResamplerState state;
    size_t out_count = 0;
    chord::Status status =
        decimate(in, out, state, factor, out_count, kfr::sample_rate_conversion_quality::normal);
    EXPECT_EQ(status, chord::Status::OK);
    EXPECT_EQ(out_count, expected);

    auto converter = kfr::sample_rate_converter<float>(kfr::sample_rate_conversion_quality::normal, 1, factor);
    converter.process(ref, in);

    for (size_t i = 0; i < expected; ++i) {
        EXPECT_NEAR(out[i], ref[i], 1e-5f);
    }
}

TEST(DecimateTest, StreamingContinuity) {
    const size_t factor = 4;
    const size_t input_size = 256;
    const auto quality = kfr::sample_rate_conversion_quality::normal;

    univector<float> in(input_size);
    for (size_t i = 0; i < in.size(); ++i) {
        in[i] = static_cast<float>(i) * 0.01f;
    }

    ResamplerState state_chunked;
    auto ref_converter = kfr::sample_rate_converter<float>(quality, 1, factor);

    std::vector<float> out_values;
    std::vector<float> ref_values;

    size_t offset = 0;
    while (offset < in.size()) {
        const size_t chunk = std::min<size_t>(31, in.size() - offset);
        auto in_slice = in.slice(offset, chunk);

        const size_t chunk_out = static_cast<size_t>(
            ref_converter.output_size_for_input(static_cast<kfr::i64>(chunk)));
        univector<float> ref_chunk(chunk_out);
        univector<float> out_chunk(chunk_out);

        ref_converter.process(ref_chunk, in_slice);
        size_t produced = 0;
        chord::Status status_chunk =
            decimate(in_slice, out_chunk, state_chunked, factor, produced, quality);
        EXPECT_EQ(status_chunk, chord::Status::OK);
        EXPECT_EQ(produced, chunk_out);

        ref_values.insert(ref_values.end(), ref_chunk.begin(), ref_chunk.end());
        out_values.insert(out_values.end(), out_chunk.begin(), out_chunk.end());

        offset += chunk;
    }

    ASSERT_EQ(out_values.size(), ref_values.size());
    for (size_t i = 0; i < ref_values.size(); ++i) {
        EXPECT_NEAR(out_values[i], ref_values[i], 1e-5f);
    }
}

TEST(DecimateTest, OutputTooSmall) {
    univector<float> in(32, 1.0f);
    univector<float> out(1);
    ResamplerState state;
    size_t out_count = 0;

    chord::Status status = decimate(in, out, state, 2, out_count);

    EXPECT_EQ(status, chord::Status::OUTPUT_TOO_SMALL);
}

TEST(DecimateTest, InvalidFactor) {
    univector<float> in(4, 1.0f);
    univector<float> out(4);
    ResamplerState state;
    size_t out_count = 0;

    chord::Status status = decimate(in, out, state, 0, out_count);

    EXPECT_EQ(status, chord::Status::INVALID_PARAM);
}

TEST(DecimateTest, EmptyInput) {
    univector<float> in;
    univector<float> out;
    ResamplerState state;
    size_t out_count = 0;

    chord::Status status = decimate(in, out, state, 2, out_count);

    EXPECT_EQ(status, chord::Status::INPUT_TOO_SMALL);
}

TEST(UpsampleTest, MatchesKfrConverter) {
    const float sample_rate = 1000.0f;
    const float tone = 50.0f;
    const size_t factor = 2;

    univector<float> in(128);
    for (size_t i = 0; i < in.size(); ++i) {
        in[i] = std::sin(2.0f * kfr::c_pi<float> * tone * static_cast<float>(i) / sample_rate);
    }

    const size_t expected = upsample_output_size(in.size(), factor);
    univector<float> out(expected);
    univector<float> ref(expected);

    ResamplerState state;
    size_t out_count = 0;
    chord::Status status =
        upsample(in, out, state, factor, out_count, kfr::sample_rate_conversion_quality::normal);

    EXPECT_EQ(status, chord::Status::OK);
    EXPECT_EQ(out_count, expected);

    auto converter = kfr::sample_rate_converter<float>(kfr::sample_rate_conversion_quality::normal, factor, 1);
    converter.process(ref, in);

    for (size_t i = 0; i < expected; ++i) {
        EXPECT_NEAR(out[i], ref[i], 1e-5f);
    }
}
