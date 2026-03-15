#include <chord/dsp/pulse_design.hpp>

#include <kfr/math.hpp>

#include <algorithm>
#include <cmath>
#include <gtest/gtest.h>

TEST(PulseDesignTest, RRC) {
    size_t span = 5;
    size_t sps = 4;
    size_t length = span * sps + 1;
    kfr::univector<float> out(length);

    chord::Status status =
        chord::dsp::design_pulse_shape(chord::dsp::PulseType::RRC, span, sps, 0.35f, out);

    EXPECT_EQ(status, chord::Status::OK);

    // Ensure symmetry and valid bounds
    for (size_t i = 0; i < length / 2; ++i) {
        EXPECT_NEAR(out[i], out[length - 1 - i], 1e-4f);
        EXPECT_FALSE(std::isnan(out[i]));
        EXPECT_FALSE(std::isinf(out[i]));
    }

    // Ensure normalized energy
    float energy = kfr::sum(kfr::sqr(out));
    EXPECT_NEAR(energy, 1.0f, 1e-3f);

    // Ensure center is valid
    size_t center = length / 2;
    EXPECT_FALSE(std::isnan(out[center]));
}

TEST(PulseDesignTest, RC) {
    size_t span = 6;
    size_t sps = 4;
    size_t length = span * sps + 1;
    kfr::univector<float> out(length);

    chord::Status status =
        chord::dsp::design_pulse_shape(chord::dsp::PulseType::RC, span, sps, 0.25f, out);

    EXPECT_EQ(status, chord::Status::OK);

    // Ensure symmetry and valid bounds
    for (size_t i = 0; i < length / 2; ++i) {
        EXPECT_NEAR(out[i], out[length - 1 - i], 1e-4f);
        EXPECT_FALSE(std::isnan(out[i]));
        EXPECT_FALSE(std::isinf(out[i]));
    }

    // Ensure normalized energy
    float energy = kfr::sum(kfr::sqr(out));
    EXPECT_NEAR(energy, 1.0f, 1e-3f);

    // Zero-crossing check (Nyquist criterion) - expect 0 at symbol boundaries
    size_t center = length / 2;
    for (size_t i = 1; i <= span / 2; ++i) {
        EXPECT_NEAR(out[center + i * sps], 0.0f, 1e-3f);
        EXPECT_NEAR(out[center - i * sps], 0.0f, 1e-3f);
    }
}

TEST(PulseDesignTest, Gaussian) {
    size_t span = 5;
    size_t sps = 4;
    size_t length = span * sps + 1;
    kfr::univector<float> out(length);

    chord::Status status =
        chord::dsp::design_pulse_shape(chord::dsp::PulseType::Gaussian, span, sps, 0.3f, out);

    EXPECT_EQ(status, chord::Status::OK);

    // Ensure symmetry and valid bounds
    for (size_t i = 0; i < length / 2; ++i) {
        EXPECT_NEAR(out[i], out[length - 1 - i], 1e-4f);
        EXPECT_FALSE(std::isnan(out[i]));
        EXPECT_FALSE(std::isinf(out[i]));
        // Gaussian should be positive
        EXPECT_GT(out[i], 0.0f);
    }

    // Ensure normalized energy
    float energy = kfr::sum(kfr::sqr(out));
    EXPECT_NEAR(energy, 1.0f, 1e-3f);
}

TEST(PulseDesignTest, OutputTooSmallReturnsEarly) {
    size_t span = 5;
    size_t sps = 4;
    size_t length = span * sps + 1;
    kfr::univector<float> out(length - 1);

    std::fill(out.begin(), out.end(), 3.0f);
    chord::Status status =
        chord::dsp::design_pulse_shape(chord::dsp::PulseType::RRC, span, sps, 0.35f, out);

    EXPECT_EQ(status, chord::Status::OUTPUT_TOO_SMALL);

    for (float value : out) {
        EXPECT_EQ(value, 3.0f);
    }
}

TEST(PulseDesignTest, SpanZeroSingleTap) {
    size_t span = 0;
    size_t sps = 4;
    size_t length = span * sps + 1;
    kfr::univector<float> out(length);

    chord::Status status =
        chord::dsp::design_pulse_shape(chord::dsp::PulseType::RC, span, sps, 0.25f, out);

    EXPECT_EQ(status, chord::Status::OK);

    EXPECT_EQ(out.size(), 1u);
    EXPECT_NEAR(out[0], 1.0f, 1e-4f);
}

TEST(PulseDesignTest, RrcSingularityIndices) {
    size_t span = 4;
    size_t sps = 4;
    float beta = 0.5f;
    size_t length = span * sps + 1;
    kfr::univector<float> out(length);

    chord::Status status =
        chord::dsp::design_pulse_shape(chord::dsp::PulseType::RRC, span, sps, beta, out);

    EXPECT_EQ(status, chord::Status::OK);

    float offset = static_cast<float>(sps) / (4.0f * beta);
    size_t center = length / 2;
    size_t idx1 = static_cast<size_t>(static_cast<float>(center) - offset);
    size_t idx2 = static_cast<size_t>(static_cast<float>(center) + offset);

    EXPECT_FALSE(std::isnan(out[idx1]));
    EXPECT_FALSE(std::isnan(out[idx2]));
    EXPECT_FALSE(std::isinf(out[idx1]));
    EXPECT_FALSE(std::isinf(out[idx2]));
    EXPECT_NEAR(out[idx1], out[idx2], 1e-4f);
}

TEST(PulseDesignTest, SpsZeroReturnsEarly) {
    size_t span = 4;
    size_t sps = 0;
    kfr::univector<float> out(1);

    std::fill(out.begin(), out.end(), 2.0f);
    chord::Status status =
        chord::dsp::design_pulse_shape(chord::dsp::PulseType::RC, span, sps, 0.25f, out);

    EXPECT_EQ(status, chord::Status::DIVIDE_BY_ZERO);

    for (float value : out) {
        EXPECT_EQ(value, 2.0f);
    }
}

TEST(PulseDesignTest, GaussianBetaZeroReturnsEarly) {
    size_t span = 4;
    size_t sps = 4;
    size_t length = span * sps + 1;
    kfr::univector<float> out(length);

    std::fill(out.begin(), out.end(), 2.0f);
    chord::Status status =
        chord::dsp::design_pulse_shape(chord::dsp::PulseType::Gaussian, span, sps, 0.0f, out);

    EXPECT_EQ(status, chord::Status::INVALID_PARAM);

    for (float value : out) {
        EXPECT_EQ(value, 2.0f);
    }
}
