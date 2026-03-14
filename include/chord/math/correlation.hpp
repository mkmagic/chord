#pragma once

#include <kfr/all.hpp>

namespace chord::math {

/**
 * @brief Computes the sum of conjugate lagged products.
 *
 * Computes $\\sum_{n=lag}^{N-1} x[n]  x[n - lag]^*$.
 * This evaluates a single point of the digital auto-correlation function.
 * Extremely useful for finding the Carrier Frequency Offset (CFO) linearly,
 * by taking the angle of the resulting sum.
 *
 * @param input Read-only view of the complex input signal.
 * @param lag The time lag (in samples) to compare against.
 * @return The accumulated complex sum.
 */
kfr::complex<float> sum_conjugate_product(kfr::univector_ref<const kfr::complex<float>> input,
                                          size_t lag = 1);

}  // namespace chord::math
