#pragma once

#include <kfr/all.hpp>

namespace chord::math {

/**
 * @brief Multiplies a complex signal by the conjugate of another.
 *
 * In RF and SDR, "multiplying by a conjugate" is the mathematical equivalent of finding
 * the phase difference between two signals. This function is essential for cross-correlation,
 * measuring frequency offset, and building FM or QPSK demodulators.
 *
 * @param in1 The first input signal vector.
 * @param in2 The second input signal vector to be conjugated.
 * @param out The resulting vector containing `in1 * conj(in2)`.
 */
void conjugate_multiply(kfr::univector_ref<const kfr::complex<float>> in1,
                        kfr::univector_ref<const kfr::complex<float>> in2,
                        kfr::univector_ref<kfr::complex<float>> out);

/**
 * @brief Computes the squared magnitude (I^2 + Q^2) of a complex signal.
 *
 * This function calculates the absolute power (or squared distance from zero) of a complex
 * IQ signal. Unlike standard magnitude, it skips the expensive square-root operation, making
 * it incredibly fast. It is heavily used to detect whether a signal exceeds a certain power
 * threshold.
 *
 * @param in Read-only view of the complex IQ input signal.
 * @param out Write view where the squared magnitude values will be stored.
 */
void magnitude_squared(kfr::univector_ref<const kfr::complex<float>> in,
                       kfr::univector_ref<float> out);

}  // namespace chord::math
