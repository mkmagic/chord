#include "chord/dsp/spectrum.hpp"

#include <gtest/gtest.h>

TEST(SpectrumTest, PSDEstimation) {
    size_t n_fft = 64;
    kfr::univector<kfr::complex<float>> input(n_fft, {1.0f, 0.0f});
    kfr::univector<float> out(n_fft);

    chord::dsp::estimate_psd(input, n_fft, kfr::window_type::rectangular, out);

    // Basic sanity check
    EXPECT_GT(out[0], 0.0f);
}
