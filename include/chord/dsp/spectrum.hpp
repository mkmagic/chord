#pragma once

#include <kfr/all.hpp>

namespace chord::dsp {

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
 */
void estimate_psd(kfr::univector_ref<const kfr::complex<float>> input,
                  size_t n_fft,
                  kfr::window_type window_type,
                  kfr::univector_ref<float> out);

}  // namespace chord::dsp
