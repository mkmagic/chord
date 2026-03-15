#include <chord/dsp/pulse_design.hpp>

#include <kfr/math.hpp>

#include <cmath>
#include <gtest/gtest.h>

TEST(PulseDesignTest, RRC) {
    size_t span = 5;
    size_t sps = 4;
    size_t length = span * sps + 1;
    kfr::univector<float> out(length);

    chord::dsp::design_pulse_shape(chord::dsp::PulseType::RRC, span, sps, 0.35f, out);

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

    chord::dsp::design_pulse_shape(chord::dsp::PulseType::RC, span, sps, 0.25f, out);

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

    chord::dsp::design_pulse_shape(chord::dsp::PulseType::Gaussian, span, sps, 0.3f, out);

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
