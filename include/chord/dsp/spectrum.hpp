#pragma once

#include <chord/error/status.h>

#include <kfr/all.hpp>

namespace chord::dsp {

/**
 * @brief Reusable workspace for PSD estimation.
 *
 * Holds FFT plan and scratch buffers so callers can avoid repeated allocations
 * across successive `estimate_psd` calls. Callers must size members to match
 * the requested FFT size and window length.
 */
struct SpectrumPsdWorkspace {
    /// Cached FFT size corresponding to the current plan and buffers.
    size_t n_fft = 0;
    /// FFT plan reused across calls for the cached FFT size.
    kfr::dft_plan<float> plan;
    /// Zero-padded complex input buffer used for in-place DFT.
    kfr::univector<kfr::complex<float>> temp;
    /// Window coefficients sized to the processed input length.
    kfr::univector<float> win;
    /// Scratch buffer required by the KFR DFT plan.
    kfr::univector<kfr::u8> temp_buffer;
};

/**
 * @brief Estimates Power Spectral Density (PSD) using a basic windowed periodogram.
 *
 * Computes the squared magnitude of the DFT of the windowed input signal, creating
 * an estimate of the signal's spectral density. Useful for evaluating out-of-band
 * emissions or Occupied Bandwidth.
 *
 * @param input Read-only view of the complex input signal.
 * @param n_fft The FFT size (number of bins). Processing is capped at `min(input.size(), n_fft)`.
 * @param window_type The KFR window type (e.g., kfr::window_type::hann) to apply before FFT.
 * @param out Write view for the resulting real-valued PSD.
 * @return Status code indicating success or failure reason.
 */
Status estimate_psd(kfr::univector_ref<const kfr::complex<float>> input,
                    size_t n_fft,
                    kfr::window_type window_type,
                    kfr::univector_ref<float> out);

/**
 * @brief Estimates PSD using a reusable workspace.
 *
 * Allows callers to reuse FFT plans and scratch buffers across calls to avoid
 * repeated allocations. The workspace must be preconfigured to match `n_fft`
 * and `min(input.size(), n_fft)`; otherwise the call returns early.
 *
 * @param input Read-only view of the complex input signal.
 * @param n_fft The FFT size (number of bins). Processing is capped at `min(input.size(), n_fft)`.
 * @param window_type The KFR window type (e.g., kfr::window_type::hann) to apply before FFT.
 * @param out Write view for the resulting real-valued PSD.
 * @param workspace Reusable FFT plan and scratch buffers.
 * @return Status code indicating success or failure reason.
 */
Status estimate_psd(kfr::univector_ref<const kfr::complex<float>> input,
                    size_t n_fft,
                    kfr::window_type window_type,
                    kfr::univector_ref<float> out,
                    SpectrumPsdWorkspace& workspace);

}  // namespace chord::dsp
