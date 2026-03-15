#include <chord/dsp/mix.hpp>
#include <chord/dsp/nco.hpp>

#include <gtest/gtest.h>

using chord::dsp::frequency_translate;
using chord::dsp::mix_complex;
using chord::dsp::nco_generate_complex;
using chord::dsp::NcoState;
using namespace kfr;

TEST(MixTest, MixPassthrough) {
    univector<complex<float>> in = { {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, -1.0f} };
    univector<complex<float>> lo(in.size(), {1.0f, 0.0f});
    univector<complex<float>> out(in.size());

    chord::Status status = mix_complex(in, lo, out);

    EXPECT_EQ(status, chord::Status::OK);
    EXPECT_EQ(out[0], in[0]);
    EXPECT_EQ(out[1], in[1]);
    EXPECT_EQ(out[2], in[2]);
}

TEST(MixTest, MixNinetyDegreeShift) {
    univector<complex<float>> in = { {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} };
    univector<complex<float>> lo(in.size(), {0.0f, 1.0f});
    univector<complex<float>> out(in.size());

    chord::Status status = mix_complex(in, lo, out);

    EXPECT_EQ(status, chord::Status::OK);
    EXPECT_EQ(out[0], complex<float>(0.0f, 1.0f));
    EXPECT_EQ(out[1], complex<float>(-1.0f, 0.0f));
    EXPECT_EQ(out[2], complex<float>(-1.0f, 1.0f));
}

TEST(MixTest, MixInPlace) {
    univector<complex<float>> io = { {1.0f, 0.0f}, {2.0f, 0.0f} };
    univector<complex<float>> lo(io.size(), {0.0f, 1.0f});

    chord::Status status = mix_complex(io, lo, io);

    EXPECT_EQ(status, chord::Status::OK);
    EXPECT_EQ(io[0], complex<float>(0.0f, 1.0f));
    EXPECT_EQ(io[1], complex<float>(0.0f, 2.0f));
}

TEST(MixTest, MixOutputTooSmall) {
    univector<complex<float>> in = { {1.0f, 0.0f} };
    univector<complex<float>> lo(in.size(), {1.0f, 0.0f});
    univector<complex<float>> out;

    chord::Status status = mix_complex(in, lo, out);

    EXPECT_EQ(status, chord::Status::OUTPUT_TOO_SMALL);
}

TEST(MixTest, MixLoTooSmall) {
    univector<complex<float>> in = { {1.0f, 0.0f}, {2.0f, 0.0f} };
    univector<complex<float>> lo(1, {1.0f, 0.0f});
    univector<complex<float>> out(in.size());

    chord::Status status = mix_complex(in, lo, out);

    EXPECT_EQ(status, chord::Status::INPUT_TOO_SMALL);
}

TEST(MixTest, FrequencyTranslateToDC) {
    const float sample_rate = 8000.0f;
    const float tone = 1000.0f;

    univector<complex<float>> in(16);
    univector<complex<float>> out(in.size());

    NcoState tone_state;
    chord::Status tone_status = nco_generate_complex(in, tone_state, tone, sample_rate);
    EXPECT_EQ(tone_status, chord::Status::OK);

    NcoState translate_state;
    chord::Status status = frequency_translate(in, out, translate_state, -tone, sample_rate);

    EXPECT_EQ(status, chord::Status::OK);
    for (size_t i = 0; i < out.size(); ++i) {
        EXPECT_NEAR(out[i].real(), 1.0f, 1e-4f);
        EXPECT_NEAR(out[i].imag(), 0.0f, 1e-4f);
    }
}

TEST(MixTest, FrequencyTranslateContinuity) {
    const float sample_rate = 8000.0f;
    const float tone = 1000.0f;

    univector<complex<float>> in(32);
    NcoState tone_state;
    chord::Status tone_status = nco_generate_complex(in, tone_state, tone, sample_rate);
    EXPECT_EQ(tone_status, chord::Status::OK);

    univector<complex<float>> out_full(in.size());
    univector<complex<float>> out_chunked(in.size());

    NcoState state_full;
    NcoState state_chunked;

    chord::Status status_full = frequency_translate(in, out_full, state_full, -tone, sample_rate);
    EXPECT_EQ(status_full, chord::Status::OK);

    size_t offset = 0;
    while (offset < in.size()) {
        const size_t chunk = std::min<size_t>(5, in.size() - offset);
        auto in_slice = in.slice(offset, chunk);
        auto out_slice = out_chunked.slice(offset, chunk);
        chord::Status status_chunk =
            frequency_translate(in_slice, out_slice, state_chunked, -tone, sample_rate);
        EXPECT_EQ(status_chunk, chord::Status::OK);
        offset += chunk;
    }

    for (size_t i = 0; i < in.size(); ++i) {
        EXPECT_NEAR(out_chunked[i].real(), out_full[i].real(), 1e-5f);
        EXPECT_NEAR(out_chunked[i].imag(), out_full[i].imag(), 1e-5f);
    }
}

TEST(MixTest, FrequencyTranslateInPlaceNotSupported) {
    univector<complex<float>> io(4, {1.0f, 0.0f});
    NcoState state;

    chord::Status status = frequency_translate(io, io, state, 1000.0f, 8000.0f);

    EXPECT_EQ(status, chord::Status::INVALID_PARAM);
}

TEST(MixTest, FrequencyTranslateSampleRateZero) {
    univector<complex<float>> in(4, {1.0f, 0.0f});
    univector<complex<float>> out(in.size());
    NcoState state;

    chord::Status status = frequency_translate(in, out, state, 1000.0f, 0.0f);

    EXPECT_EQ(status, chord::Status::DIVIDE_BY_ZERO);
}

TEST(MixTest, FrequencyTranslateOutputTooSmall) {
    univector<complex<float>> in(4, {1.0f, 0.0f});
    univector<complex<float>> out(1);
    NcoState state;

    chord::Status status = frequency_translate(in, out, state, 1000.0f, 8000.0f);

    EXPECT_EQ(status, chord::Status::OUTPUT_TOO_SMALL);
}
