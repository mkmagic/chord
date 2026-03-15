#include <chord/dsp/spectrum.hpp>

#include <algorithm>
#include <gtest/gtest.h>

TEST(SpectrumTest, PSDEstimation) {
    size_t n_fft = 64;
    kfr::univector<kfr::complex<float>> input(n_fft, {1.0f, 0.0f});
    kfr::univector<float> out(n_fft);

    chord::Status status =
        chord::dsp::estimate_psd(input, n_fft, kfr::window_type::rectangular, out);

    EXPECT_EQ(status, chord::Status::OK);

    // Basic sanity check
    EXPECT_GT(out[0], 0.0f);
}

TEST(SpectrumTest, PSDOutTooSmallNoWorkspace) {
    size_t n_fft = 16;
    kfr::univector<kfr::complex<float>> input(n_fft, {1.0f, 0.0f});
    kfr::univector<float> out(n_fft - 1);

    chord::Status status =
        chord::dsp::estimate_psd(input, n_fft, kfr::window_type::rectangular, out);

    EXPECT_EQ(status, chord::Status::OUTPUT_TOO_SMALL);
}

TEST(SpectrumTest, PSDEstimationWithWorkspace) {
    size_t n_fft = 32;
    kfr::univector<kfr::complex<float>> input(n_fft, {1.0f, 0.0f});
    kfr::univector<float> out(n_fft);
    chord::dsp::SpectrumPsdWorkspace workspace;

    workspace.n_fft = n_fft;
    workspace.plan = kfr::dft_plan<float>(n_fft);
    workspace.temp.resize(n_fft);
    workspace.temp_buffer.resize(workspace.plan.temp_size);
    workspace.win.resize(n_fft);

    chord::Status status =
        chord::dsp::estimate_psd(input, n_fft, kfr::window_type::hann, out, workspace);

    EXPECT_EQ(status, chord::Status::OK);

    EXPECT_EQ(workspace.n_fft, n_fft);
    EXPECT_EQ(workspace.temp.size(), n_fft);
    EXPECT_EQ(workspace.temp_buffer.size(), workspace.plan.temp_size);
    EXPECT_GT(out[0], 0.0f);
}

TEST(SpectrumTest, PSDZeroProcSize) {
    size_t n_fft = 16;
    kfr::univector<kfr::complex<float>> input(0);
    kfr::univector<float> out(n_fft);
    chord::dsp::SpectrumPsdWorkspace workspace;

    std::fill(out.begin(), out.end(), 1.0f);
    chord::Status status =
        chord::dsp::estimate_psd(input, n_fft, kfr::window_type::rectangular, out, workspace);

    EXPECT_EQ(status, chord::Status::INPUT_TOO_SMALL);

    for (float value : out) {
        EXPECT_EQ(value, 1.0f);
    }
}

TEST(SpectrumTest, PSDOutTooSmall) {
    size_t n_fft = 16;
    kfr::univector<kfr::complex<float>> input(n_fft, {1.0f, 0.0f});
    kfr::univector<float> out(n_fft - 1);
    chord::dsp::SpectrumPsdWorkspace workspace;

    std::fill(out.begin(), out.end(), 1.0f);
    chord::Status status =
        chord::dsp::estimate_psd(input, n_fft, kfr::window_type::rectangular, out, workspace);

    EXPECT_EQ(status, chord::Status::OUTPUT_TOO_SMALL);

    for (float value : out) {
        EXPECT_EQ(value, 1.0f);
    }
}

TEST(SpectrumTest, PSDWorkspaceMismatchReturnsEarly) {
    size_t n_fft = 32;
    kfr::univector<kfr::complex<float>> input(n_fft, {1.0f, 0.0f});
    kfr::univector<float> out(n_fft);
    chord::dsp::SpectrumPsdWorkspace workspace;

    std::fill(out.begin(), out.end(), 2.0f);
    workspace.n_fft = n_fft;
    workspace.plan = kfr::dft_plan<float>(n_fft);
    workspace.temp.resize(n_fft);
    workspace.temp_buffer.resize(workspace.plan.temp_size);
    workspace.win.resize(n_fft - 1);

    chord::Status status =
        chord::dsp::estimate_psd(input, n_fft, kfr::window_type::rectangular, out, workspace);

    EXPECT_EQ(status, chord::Status::WORKSPACE_MISMATCH);

    for (float value : out) {
        EXPECT_EQ(value, 2.0f);
    }
}
