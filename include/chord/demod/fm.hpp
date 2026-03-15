#pragma once

#include <chord/error/status.h>

#include <kfr/all.hpp>

namespace chord::demod {

/**
 * @brief State container for streaming FM demodulators (quadrature discriminators).
 */
struct FmDemodulatorState {
    kfr::complex<float> previous_sample{0.0f, 0.0f};
};

/**
 * @brief Performs FM Baseband Demodulation across a continuous complex stream.
 *
 * This function functions as a Quadrature Discriminator. For each complex IQ input
 * sample `x[n]`, it calculates the phase difference relative to the previous sample
 * `x[n-1]`. The resulting instantaneous phase difference perfectly matches the
 * underlying real audio/data waveform that was FM modulated.
 *
 * It combines C++ manual state tracking at the buffer boundary (`out[0]`)
 * with blazing-fast native KFR SIMD expression templates for the remainder
 * of the buffer (`out[1...N]`).
 *
 * @param in Read-only view of the complex IQ baseband signal.
 * @param out Write view where the real demodulated signal will be stored.
 * @param state State maintained across buffer calls to ensure continuity of `x[n-1]`.
 * @param gain The FM gain scaler. Usually set to `samp_rate / (2 * PI * max_deviation)`.
 * @return Status code indicating success or failure reason.
 */
Status fm_demodulate(kfr::univector_ref<const kfr::complex<float>> in,
                     kfr::univector_ref<float> out,
                     FmDemodulatorState& state,
                     float gain = 1.0f);

}  // namespace chord::demod
